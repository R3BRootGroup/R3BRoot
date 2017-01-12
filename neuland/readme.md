# NeuLAND module for r3broot

## Components
Code for the NeuLAND Detector is split into different components:

- simulation
- digitizing
- clustering
- reconstruction
- test
- unpack
- shared

Note that in contrast to the old NeuLAND and LAND code in the /land/ directory, the spelling in e.g. class names is "Neuland", with a small "l".


### Simulation

- [`R3BNeuland`](#detector-class) Main detector including light generation -> NeulandPoints


### Tasks

These tasks are derived from FairTask and used in the steering macros.

- `R3BNeulandMCMon`: Control histograms for MonteCarlo data (NeulandPoints)
- [`R3BNeulandDigitizer`](#digitizing): Detector response, NeulandPoints -> NeulandDigis
- `R3BNeulandDigiMon`: Control histograms for digitized data (NeulandDigis)
- [`R3BNeulandClusterFinder`](#clustering): Clustering of digitized data, NeulandDigis -> NeulandClusters
- `R3BNeulandClusterMon`: ControlHistograms for clusters (NeulandClusters)
- [`R3BNeulandNeutronReconstruction`](#event-reconstruction): Determination of Neutron first interaction points, NeulandClusters -> NeulandNeutrons
- `R3BNeulandNeutronReconstructionEvaluation`: Comparison of reconstructed with actual neutron interaction points.


### Engines

For separation of the actual physics/business logic from the IO-managing tasks.

- `Neuland::DigitizingEngine`
- [`Neuland::ClusteringEngine`](#implementation-of-the-handshake-chain-clustering)
- `Neuland::ReconstructionEngine`

### Data Storage

Note that the classes indented for storing data (derived from TObject for usage with TClonesArray) are located in `r3bdata/neulandData`. 

- `R3BNeulandPoint`: Basic MonteCarlo energy depositions and light yield, see, [Detector Class](#detector-class)
- `R3BNeulandDigi`: Combined NeulandPoints, digitized with detector response, see [Digitizing](#digitizing). Also indented for mapped and calibrated experimental data.
- `R3BNeulandCluster`: Clusters consisting out of NeulandDigis that belong together according to clustering conditions, see [Clustering](#clustering)
- `R3BNeulandNeutron`: Position, Energy and Time information for neutron interactions found by event reconstruction processes


### Configuration Storage

- `R3BNeulandGeoPar`: The complete NeuLAND GeoNode used in the Simulation
- `R3BNeulandNeutron2DPar`: Cuts used for the [2D Method](#the-2d-method-for-neutron-multiplicity-determination) to determine neutron multiplicities.


### Auxiliary Classes

- `R3BNeulandContFact`: Container Factory for the configuration storage classes (pure boilerplate)
- `R3BNeulandVisualizer`: 3D display of events, prepared by the `-Mon` tasks. (Work in progress)
- [`Neuland::Neutron2DCalibr`](#calibration): Calibration of cuts for the 2D neutron multiplicity method


## Detector Class

`R3BNeuland` is the main class derived from `R3BDetector` for use in simulations. Note that it does not include the geometry itself, but takes `neuland_v2_` geometry files from `/geometry/`. Suitable geometry files require proper naming of the active volume and copy numbers. See [Geometry](#geometry)

It fills to main outputs for each simulated event:
- NeulandPoints (`R3BNeulandPoint`), each representing energy deposition and light yield of a track in a paddle.
- NeulandPrimaryNeutronInteractionPoints (`FairMCPoint`), each representing the first interaction points of primary neutrons for later comparison to the reconstructed neutrons.

Control histograms are extracted to `R3BNeulandMCMon`.


## Digitizing

The Digitizing task `R3BNeulandDigitizer` handles the Input and Output of data while invoking the `Neuland::DigitizingEngine` for the actual processing. The Digitizing Engine is independent of any (Fair-/R3B-)Root class except the random number generator and can be reused in other projects if necessary.

The task takes the TClonesArray NeulandPoints(`R3BNeulandPoint`) and fills TClonesArray NeulandDigis(`R3BNeulandDigi`). In addition the configuration storage `NeulandGeoPar`

The digitizing engine is awaiting further changes reflecting changes in the NeuLAND electronics. (Work in Progress)

In addition, some control histograms are created, while most control histograms are extracted to `R3BNeulandDigiMon`.


## Clustering

Clustering is the process of grouping Objects together by a specified condition. 

The task `R3BNeulandClusterFinder` uses the  implementation in `Neuland::ClusteringEngine` of what can be called *handshake-chain clustering*, where a cluster is finished if all of the Digis in it have no neighbor that is not in the cluster.

The task takes the TClonesArray NeulandDigis(`R3BNeulandDigi`) and fills TClonesArray NeulandClusters(`R3BNeulandCluster`).

Control histograms are located  to `R3BNeulandClusterMon`.


### Implementation of the handshake-chain clustering

The implementation is a template, thus any type of object can be clustered together.
For this example, integers are used. Starting point is an unsorted vector of integers and the clustering condition is a difference of 1 or less. For the clustering process, several iterators referencing certain points in the vector are needed:

- The *begin* of the current part of the vector to look at
- A *moving_divider* that separates the clustered part from the rest of the unclustered part
- The fixed *end* of the vector
- And the iterator to the object currently looked at, here called *a*

```C++
Tit moving_partition(const Tit begin, Tit moving_divider, const Tit end) const
{
	for (Tit a = begin; a != moving_divider; a++)
	{
		moving_divider = std::partition(moving_divider, end, [&](const T& b) {return f(*a, b);});
	}
    return moving_divider;
}
```

In the beginning, the vector is completely unclustered and a new cluster is started with the first element:

	{ 28, 13, 23, 22, 15, 16, 3, 6, 4, 26, 10, 11, 19, 8, 29, 12, 25, 30, 17, 18, 24 }
	   │   │                                                                         │
	   │   └ moving_divider                                                          └ end
	   └ begin, a

Now, each object from the moving_divider to the end is compared with *a*, which currently is the first element, and if the clustering condition is fulfilled, moved next to it. This is done by std::partition. Now the *moving_divider* is set to the end of this partitioned part, and the next element between the current *a* and the *moving_divider* becomes the new *a*, which the unclustered part is compared to:

	{ 28, 29, 30, 22, 15, 16, 3, 6, 4, 26, 10, 11, 19, 8, 23, 12, 25, 13, 17, 18, 24 }
	   │   │       │                                                                 │
	   │   a       └ moving_divider                                                  └ end
	   └ begin

This continues until the *moving_divider* is reached, at this point the cluster is finished and extracted, and a new cluster is started:

	{ xx, xx, xx, 22, 15, 16, 3, 6, 4, 26, 10, 11, 19, 8, 23, 12, 25, 13, 17, 18, 24 }
	               │   │                                                             │
	               │   └ moving_divider                                              └ end
	               └ begin, a

Note that the *moving_divider* does move every time std::partition adds new objects to the cluster, thus also these times are iterated over:


	{ xx, xx, xx, 22, 23, 16, 3, 6, 4, 26, 10, 11, 19, 8, 15, 12, 25, 13, 17, 18, 24 }
	               │   │   │                                                         │
	               │   a   └ moving_divider                                          └ end
	               └ begin
	{ xx, xx, xx, 22, 23, 24, 3, 6, 4, 26, 10, 11, 19, 8, 15, 12, 25, 13, 17, 18, 16 }
	               │       │  │                                                      │
	               │       a  └ moving_divider                                       └ end
	               └ begin
	{ xx, xx, xx, 22, 23, 24, 25, 6, 4, 26, 10, 11, 19, 8, 15, 12, 3, 13, 17, 18, 16 }
	               │           │  │                                                  │
	               │           a  └ moving_divider                                   └ end
	               └ begin

Note that for this process, the order of the objects is irrelevant. The algorithm will always find all elements that belong together.

In this example, the result is:

	{ 28, 29, 30 }, { 22, 23, 24, 25, 26 }, { 4, 3 }, { 10, 11, 12, 13 }, { 8 }, { 19, 18, 17, 16, 15 }, { 6 }


## The 2D-method for neutron multiplicity determination

It was found that plotting the number of clusters vs the total energy is an acceptable way of determining the neutron multiplicities, at least for large numbers of planes.


### Calibration

This method needs to be calibrated. In this process, the cuts applied to the 2D histogram need to be created and adjusted such that the "best" result is obtained. This is handled by the `Neuland::Neutron2DCalibr` class, which is not a task as specifically set inputs are needed (events have different numbers of primary neutrons). Cuts created by this process are stored in `R3BNeulandNeutron2DPar`.

![Example of 2D Cuts](docs/1400cm_30dp.neutroncuts.png "2D neutron cuts example for 30 double planes")

At this point, efficiency matrices can already be created for the calibration dataset, e.g.:

| 600 MeV |  1n       |  2n       |  3n       |  4n       |
|--------:|-----------|-----------|-----------|-----------|
| **0n**  |  0.06     |  0        |  0        |  0        |
| **1n**  |  **0.90** |  0.16     |  0.01     |  0        |
| **2n**  |  0.04     |  **0.76** |  0.23     |  0.04     |
| **3n**  |  0        |  0.07     |  **0.68** |  0.32     |
| **4n**  |  0        |  0        |  0.08     |  **0.56** |

With the number of generated neutrons on the columns and the detected neutron multiplicity in the rows. Note that in this example, the chance to detect no neutron whatsoever is only 6% in the case of one incident neutron. In any other case, the total detection efficiency is 100%.

The cuts are saved in the parameter file via `R3BNeulandNeutron2DPar`. Provided with the total energy and number of clusters, this class then can return the neutron multiplicity.


### Event Reconstruction

Provided with the neutron multiplicity parameter *nN*, the task `R3BNeulandNeutronReconstruction` can try to find the neutron interaction points from the TClonesArray NeulandClusters (`R3BNeulandCluster`). Currently, `RecoTDR` and `ClusterScoring` are available. These positions are then saved to the TClonesArray NeulandNeutrons (`R3BNeulandNeutron`).

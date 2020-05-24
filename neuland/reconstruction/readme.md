# Neuland Reconstruction

Event reconstruction covers two parts: *Multiplicity* and first interaction points of *neutrons*. There are several methods to determine both individually. For example, the combination of the calorimetric multiplicity method `R3BNeulandMultiplictyCalorimetric` and cluster selection via R-Value with `R3BNeulandNeutronsRValue` is equivalent to the classic TDR reconstruction.

Multiplicity methods and neutron methods can be mixed and matched freely, however most neutron methods require multiplicity as input. Most methods require some sort of calibration.


## Overview:

- `multiplicity/R3BNeulandMultiplicityBayes` Simple probability calculation from event properties
- `multiplicity/R3BNeulandMultiplicityCalorimetric` Classic calorimetric cuts
- `multiplicity/R3BNeulandMultiplicityCheat` Get number of reacted neutrons from simulation
- `multiplicity/R3BNeulandMultiplicityFixed` Set a fixed value to each event
- `multiplicity/R3BNeulandMultiplicityScikit` Use a pre-trained pickled scikit-learn model
- `neutrons/R3BNeulandNeutronsCheat` Get correct neutrons from simulation
- `neutrons/R3BNeulandNeutronsRValue` Classic R-Value sorting for clusters
- `neutrons/R3BNeulandNeutronsScikit` Use a pre-trained pickled scikit-learn model


## Multiplicity

Multiplicity is saved in `R3BNeulandMultiplicity` data structures for each event. This contains an array to store the probability for each possible multiplicity value, e.g., `[0, 0, 0.000911, 0.105381, 0.893707, 0, 0]`. The provided `GetMultiplicity` method would return `4` in this case. However, you might want to introduce filters for less clear-cut cases.
Note that some algorithms, like the calorimetric one, will provide the multiplicity as one-hot: `[0, 0, 0, 0, 1, 0, 0]`.


### Calorimetric method

It was found that plotting the number of clusters vs the total energy is an acceptable way of determining the neutron multiplicities, at least for large numbers of planes.

This method needs to be calibrated. In this process, the cuts applied to the 2D histogram need to be created and adjusted such that the "best" result is obtained. This is handled by the `Neuland::Neutron2DCalibr` class, which is not a task as specifically set inputs are needed (events have different numbers of primary neutrons). Cuts created by this process are stored in `R3BNeulandMultiplicityCalorimetricPar`.

![Example of 2D Cuts](../docs/1400cm_30dp.neutroncuts.png "2D neutron cuts example for 30 double planes")

At this point, efficiency matrices can already be created for the calibration dataset, e.g.:

| 600 MeV |  1n       |  2n       |  3n       |  4n       |
|--------:|-----------|-----------|-----------|-----------|
| **0n**  |  0.06     |  0        |  0        |  0        |
| **1n**  |  **0.90** |  0.16     |  0.01     |  0        |
| **2n**  |  0.04     |  **0.76** |  0.23     |  0.04     |
| **3n**  |  0        |  0.07     |  **0.68** |  0.32     |
| **4n**  |  0        |  0        |  0.08     |  **0.56** |

With the number of generated neutrons on the columns and the detected neutron multiplicity in the rows. Note that in this example, the chance to detect no neutron whatsoever is only 6% in the case of one incident neutron. In any other case, the total detection efficiency is 100%.

The cuts are saved in the parameter file via `R3BNeulandMultiplicityCalorimetricPar`. Provided with the total energy and number of clusters, this class then can return the neutron multiplicity.


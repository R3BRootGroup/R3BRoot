# NeuLAND Tetraneutron Simulations and Reconstruction

A very difficult, but realistic(TM) testcase for NeuLAND detector performance: Identify and reconstruct four neutrons for a beam energy of 600AMeV and and excitaion energy E_rel = 100keV with a low mass fragment.

Uses all parts of the NeuLAND Simulation and Reconstruction code.

Note that the generated neutrons form an extremely narrow cone, at a distance of 14m, basically only the four central bars are hit. While this does not affect the neutron interaction probability too much, it severely handicaps finding all the interaction points, as many of these can be absorbed in one cluster.

## Available scripts:

- Step 0: Creating geometry and input files

	Creates the required `*.geo.root` NeuLAND geometries with different number of doubleplanes and distances, as well as the ascii-input file `4He_4n_600AMeV_100keV.dat`.

- Step 1: Simulation

- Step 2: Digitizing and Clustering

- Step 3: Calibration for 2DCalibr Method

	Note: Requires pre-build parameter file including 1n-4n cuts, e.g. from `matrices/`. Also creates a fake parameter file that can be used as substitute.

- Step 4: Reconstruct neutron hits with different methods

	Invokes different reconstruction methods and writes the reconstructed neutron hits to appropriately named containers.
	Requires a parameter file for 2DCalibr and an active neural network neutron multiplicity reco-server, see `neuralnetwork/`.

- Step 5: Evaluate the reconstruction

- Plots

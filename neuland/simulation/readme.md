# Neuland Simulation


## Detector
`R3BNeuland` is the main class derived from `R3BDetector` for use in simulations. Note that it does not include the geometry itself, but takes `neuland_v3_` geometry files from `/geometry/`. Suitable geometry files require proper naming of the active volume and copy numbers.

Output for each simulated event:
- NeulandPoints (`R3BNeulandPoint`), each representing energy deposition and light yield of a track in a paddle.


## First interaction points
The first interaction points and the hits corresponding to these are required to train the reconstruction models.

These points were previously determined in `PostTrack` in `R3BNeuland`, however this method missed about 10% of primary interactions.

Instead, the interactions points are now determined by backtracking through the MCPoints, implemented in `R3BPimaryInteractionFinder`:

Input:
- MCTrack (`R3BMCTrack`)
- NeulandPoints (`R3BNeulandPoint`)
- NeulandHits (`R3BNeulandHit`)

Output:
- NeulandPrimaryPoints (`R3BNeulandPoint`)
- NeulandPrimaryHits (`R3BNeulandHit`)

In addition the `R3BNeulandPrimaryClusterFinder` separates clusters in primary (and secondary) clusters:

Input: 
- NeulandPrimaryHits (`R3BNeulandHit`)
- NeulandClusters (`R3BNeulandCluster`)

Output:
- NeulandPrimaryClusters (`R3BNeulandCluster`)
- NeulandSecondaryClusters (`R3BNeulandCluster`)

Example difference between Backtracking and PostTrack:

|       | 1 | 2 | 3 |   4 |    5 |
|-------|---|---|---|-----|------|
| **1** | 0 | 0 | 0 |   0 |    0 |
| **2** | 0 | 0 | 0 |   0 |    0 |
| **3** | 0 | 0 | 2 |  24 |   50 |
| **4** | 0 | 0 | 0 | 293 | 1011 |
| **5** | 0 | 0 | 0 |   0 | 8620 |
(9681 5n events with backtracking vs 8620 5n events with PostTrack)


## Control histograms
Control histograms are filled in `R3BNeulandMCMon`.
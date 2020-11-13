## Generators and Distributions
You can run a simulation with beam and relative energy distributions using R3BPhaseSpaceGenerator and R3BParticleGenerator.

You can change the vertex, beam spread and beta distributions via the respective setter methods (e.g. "SetVertexDistribution").
Those methods take objects of the template type R3BDistribution as an argument.

You can assign distributions using R3BDistribution1D, R3BDistribution2D or R3BDistribution3D.

### Examples

#### Delta Distribution
The following code will result in a fixed Beam beta of 0.5:
```c++
    r3bgen->SetBeamEnergyDistribution(R3BDistribution1D::Delta(0.5));
``` 

#### Flat Distribution
The following code will result in a flat Beam beta distribution between 0.45 and 0.55:
```c++
    r3bgen->SetBeamEnergyDistribution(R3BDistribution1D::Flat(0.45, 0.55));
``` 

#### Custom Distribution
The following code will result in a x² distributed Beam Energy between beta = 0.4 and beta = 0.6 :
```c++
    constexpr Double_t Sqr(const Double_t val){
        return val*val;
    }
    [...]
    r3bgen->SetBeamBetaDistribution(R3BDistribution1D::Function(Sqr, 0.4, 0.6));
``` 

#### Multi-Dim Distribution
You can combine 1d-distributions to gain higher dimensional distributions:
```c++
    r3bgen->SetVertexDistribution({R3BDistribution1D::Delta(0.),
                                      R3BDistribution1D::Data(myHisto),
                                      R3BDistribution1D::Gaussian(0., 1.)});
``` 

### Obtaining the random Numbers
You can store the generated energies inside the tree using:
```c++
    auto beamEDist = R3BDistribution1D::Delta(500.);
    auto beamE = beamEDist.GetValueAddresses()[0]; // GetValueAddresses will return an array
    FairRootManager::Instance()->RegisterAny("Sim_BeamE_AMeV", beamE, kTRUE);
```

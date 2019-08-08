## R3BPhaseSpaceGenerator and Energy Distributions
You can run a simulation with beam and relative energy distributions using R3BPhaseSpaceGenerator.

You can change the vertex, beamspread and energy distrubutions via the respective setter methodes (e.g. "SetBeamEnergyDistribution_AMeV").
Those Methodes take objects of the template type R3BDistribution as an argument.

You can assaign distributions using R3BDistribution1D, R3BDistribution2D or R3BDistribution3D.

### Examples

#### Delta Distribution
The following code will result in a fixed Beam Energy of 500 AMev:
```c++
    r3bgen->SetBeamEnergyDistribution_AMeV(R3BDistribution1D::Delta(500));
``` 

#### Flat Distribution
The following code will result in a flat Beam Energy distribution between 400 and 600 AMeV:
```c++
    r3bgen->SetBeamEnergyDistribution_AMeV(R3BDistribution1D::Flat(400, 600));
``` 

#### Custom Distribution
The following code will result in a x² distributed Beam Energy between 400 and 600 AMeV:
```c++
    constexpr Double_t Sqr(const Double_t val){
        return val*val;
    }
    [...]
    r3bgen->SetBeamEnergyDistribution_AMeV(R3BDistribution1D::Function(Sqr, 400, 600));
``` 

#### Multi-Dim Distribution
You can combine 1d-distributions to gain higher dimensional distributions:
```c++
    r3bgen->SetVertexDistribution_cm({R3BDistribution1D::Delta(0.),
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

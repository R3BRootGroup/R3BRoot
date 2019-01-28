## R3BPhaseSpaceGenerator and Energy Distributions
You can run a simulation with beam and relative energy distributions using R3BPhaseSpaceGenerator.

There are 4 possibilities for setting the energies:
- constant number  
  The Generator will always use the passed number as the energy
- passing a lambda expression  
  Most convenient for constant or linear distributions
- passing a TF1  
  Most convenient for default distributions like gaussian etc
- passing a TGraph  
  Most convenient for distributions obtained from theory simulations
  

The beam energy and the relative energy are setup with different functions, but besides this behave absolutely the same.
Use 'SetBeamEnergyAMeV' and 'SetBeamEnergyDistAMeV' for the beam Energy and 'SetErelkeV' and 'SetErelDistkeV' for the relative Energy.

### Note
The beam energy should be given in MeV while the realtive energy should be given in keV
### Constant Number
Pass the wanted energy to the functions:
```c++
r3bgen->SetBeamEnergyAMeV(400);
r3bgen->SetErelkeV(100);
```
The code above will result in a beam energy of 400 AMeV and 100 keV relative energy.

### Lambda Expression
To use lambda expressions just call the 'Dist' functions like this:
```c++
r3bgen->SetBeamEnergyDistAMeV([](Double_t energy){ return energy*energy; }, 350, 360);
```
This code example will use a x^2 distribution from 350 AMeV to 360 AMeV.

### TF1
Just create the TF1 with the desired distribution and pass it to the 'Dist' functions.
```c++
TF1 gausDist("gausDist","gaus(0)",0,100)
gausDist.SetParameter(0, 1); // the amplitude does not matter
gausDist.SetParameter(1, 50); // the mean value
gausDist.SetParameter(2, 10); // sigma
r3bgen->SetErelDistkeV(gausDist);
```
This code example will use a gaus distribution from 0 keV to 100 keV with a mean of 50 keV and sigma = 10 keV.
If you want other boundaries, call the functions with the lower and upper boundary:
```c++
SetErelDistkeV(gausDist, minE, maxE);
```
If you pass a negative number as the upper or lower limit,
the program will find the limit of the distribution and use it instead.
### TGraph
You have to fill a TGraph with your distribution.
If you received  table-like data from your theory colleague like for example this:
```
energy1 value1
energy2 value2
...
energyn valuen
```
you can directly load the data into the TGraph:
```c++
TGraph ereldist("/path/to/distribution.dat");
```

Afterwards you have to pass the TGraph to R3BPhaseSpaceGenerator:
```c++
r3bgen->SetErelDistkeV(ereldist);
```

If you want to only simulate between two certain energies you can pass them as well:
```c++
r3bgen->SetErelDistkeV(ereldist, emin, emax);
```
If you pass a negative number as the upper or lower limit,
the program will find the limit of the distribution and use it instead.

If your data is in MeV instead of keV (or any other unit), you can use this few lines to change it to change is quickly:
```c++
TGraph ereldist("/path/to/distribution\_in\_MeV.dat");
Int_t nPoints = ereldist.GetN();
for(Int_t i = 0; i < nPoints; ++i)
    ereldist.GetX()[i] *= 1000; // 1 MeV = 1000 keV
```

### Obtaining the random Energies

You can store the generated energies inside the tree using:
```c++
auto ebeam = r3bgen->GetBeamEnergyAMeV();
auto erel = r3bgen->GetErelkeV();
auto beambeta = r3bgen->GetBeamBeta();
auto beamgamma = r3bgen->GetBeamGamma();
FairRootManager::Instance()->RegisterAny("Sim_BeamE_AMeV", ebeam, kTRUE);
FairRootManager::Instance()->RegisterAny("Sim_Erel_keV", erel, kTRUE);
FairRootManager::Instance()->RegisterAny("Sim_BeamBeta", beambeta, kTRUE);
FairRootManager::Instance()->RegisterAny("Sim_BeamGamma", beamgamma, kTRUE);
```

### Note
The Distribution will be sampled with a certain number of points (1000 by default) between the maximum and minimum energy.
Between the data points the energy will be linearly interpolated. 
This should be enough for almost every case.
If you have a strongly fluctuating distribution, you can increase the number of samples:
```c++
r3bgen->SetErelDistkeV(ereldist, -1, -1, nsamples);
```

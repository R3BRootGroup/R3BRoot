# Atima Wrapper for R3BRoot

[ATIMA](https://web-docs.gsi.de/~weick/atima/) is a program developed at GSI which calculates various physical quantities characterizing the slowing down of protons and heavy ions in matter for specific kinetic energies ranging from 1 keV/u to 450 GeV/u.  

You can get the stopping power, energy loss, energy-loss straggling, range, etc. from Atima inside R3BRoot with only a few lines of code.
For example, the following line will give you the energy loss of a proton beam @ 400 MeV in a 50mm liquid hydrogen target:

```c++
    R3BAtima::Calculate_mm(1, 1, 400, R3BAtimaTargetMaterial::LH2, 50).ELoss_AMeV;
``` 

You can define Materials using `R3BAtimaTargetMaterial` and `R3BAtimaMaterialCompound` or use predefined Materials inside `R3BAtimaTargetMaterial`.
**Please add commonly used materials which are not defined there yet!**

Since the computation takes a small amount of time, you can cache the result in a chosen range in order to increase the speed in frequent computations.

The following lines will create a cache which can be read out whithin the chosen range. The results will be interpolated from the precalculated points.

```c++
    // Energie from 100 AMeV to 200 AMeV with 10 steps
    // Distance from 0mm to 50mm with 20 steps
    auto cache = R3BAtimaCache(1, 1, {100, 200, 10}, R3BAtimaTargetMaterial::LH2, {0, 50, 20});
    auto res = cache(150, 4.9); // Get data object with 150 AMeV and 4.9mm
``` 

You can store the cache in a file in order to save the computation time.
The following lines will load the data from the file in case it exists and matches the arguments.
Otherwise it will compute the values and caches them in the file.

```c++
    auto cache = R3BAtimaCache(1, 1, {100, 200, 10}, R3BAtimaTargetMaterial::LH2, {0, 50, 20}, "temp.atima");
``` 
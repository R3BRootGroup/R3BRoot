gfortran -c normod.f
gfortran -c veclib.f
gfortran veclib.o normod.o -o normod

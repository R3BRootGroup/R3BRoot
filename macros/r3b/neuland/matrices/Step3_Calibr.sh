#!/bin/bash

# Run all simulations in "parallel" in background
COUNT=0
for NDOUBLEPLANES in $(seq 4 50); do
	# Note: The root call is extemely sensitive to the usage of ' and "
	root -l -q -b -e 'gInterpreter->AddIncludePath("'${VMCWORKDIR}'")' "calibr_mini.C+(${NDOUBLEPLANES})"
	#root -l -q -b -e 'gInterpreter->AddIncludePath("'${VMCWORKDIR}'")' "calibr.C+(${NDOUBLEPLANES})"
done

wait

# Remove junk
rm calibr_mini_C*
rm calibr_C*

#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

NEVENTS=10000
DISTANCE=1400

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# All particles, particleID, and momentum to simulate
declare -a DATA
# 600 MeV/u
#DATA[0]='neutron;2112;1.22'
#DATA[1]='proton;2212;1.22'
#DATA[2]='deuteron;1000010020;2*1.22'
#DATA[3]='triton;1000010030;3*1.22'
#DATA[4]='He3;1000020030;3*1.22'
#DATA[5]='alpha;1000020040;4*1.22'

# 300 MeV/u
DATA[0]='neutron;2112;0.808'
DATA[1]='proton;2212;0.808'
DATA[2]='deuteron;1000010020;2*0.808'
DATA[3]='triton;1000010030;3*0.808'
DATA[4]='He3;1000020030;3*0.808'
DATA[5]='alpha;1000020040;4*0.808'



# Run all simulations in "parallel" in background
COUNT=0
for NDOUBLEPLANES in $(seq 30 30); do
	for i in "${DATA[@]}"; do
		SET=(${i//;/ })
		PARTICLE=${SET[0]}
		PARTICLEID=${SET[1]}
		MOMENTUM=${SET[2]}

		# Note: The root call is extemely sensitive to the usage of ' and "
		BASENAME="${DISTANCE}cm_${NDOUBLEPLANES}dp_${PARTICLE}"
		COMMAND="Step1_Simulate.C(${NEVENTS}, \"${OUTDIR}\", \"${BASENAME}\", ${PARTICLEID}, ${MOMENTUM}, \"v2_${DISTANCE}cm_${NDOUBLEPLANES}dp\")"
		nice -n 19 root -l -q -b "${COMMAND}" &> "${OUTDIR}/${BASENAME}.sim.log" &
		echo ${COMMAND}

		# Only spawn so many processes at once
		COUNT=$((${COUNT}+1))
		if (( ${COUNT} % 30 == 0 )); then
			wait
		fi
	done
done

# Wait for all background jobs to finish
wait

# Remove junk
rm -f calor.out
rm -f flukaerr.dat
rm -f gphysi.dat

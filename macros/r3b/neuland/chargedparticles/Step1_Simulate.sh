#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

NEVENTS=100000
DISTANCE=1400

# Kill background jobs if script is terminated
trap 'echo "Stopping..."; kill $(jobs -pr) 2>/dev/null; exit' SIGINT SIGTERM
# Remove Junk on exit
trap 'echo "Cleaning..."; rm -f calor.out; rm -f flukaerr.dat; rm -f gphysi.dat; rm -f core_dump*; exit' EXIT

# All particles, particleID, and momentum to simulate
declare -a DATA
DATA[0]='neutron;2112'
DATA[1]='proton;2212'
DATA[2]='pionp;211'
DATA[3]='pion0;111'
DATA[4]='pionn;-211'
#DATA[5]='deuteron;1000010020'
#DATA[6]='triton;1000010030'
#DATA[7]='He3;1000020030'
#DATA[8]='alpha;1000020040'

# Run all simulations in "parallel" in background
COUNT=0
for NDOUBLEPLANES in $(seq 30 30); do
	for KINENERGY in $(seq 550 50 600); do
		for i in "${DATA[@]}"; do
			SET=(${i//;/ })
			PARTICLE=${SET[0]}
			PARTICLEID=${SET[1]}

			# Note: The root call is extemely sensitive to the usage of ' and "
			BASENAME="${DISTANCE}cm_${NDOUBLEPLANES}dp_${PARTICLE}_${KINENERGY}MeV"
			COMMAND="Step1_Simulate.C(${NEVENTS}, \"${OUTDIR}\", \"${BASENAME}\", ${PARTICLEID}, ${KINENERGY}, \"v2_${DISTANCE}cm_${NDOUBLEPLANES}dp\")"
			nice -n 19 root -l -q -b "${COMMAND}" &> "${OUTDIR}/${BASENAME}.sim.log" &
			echo ${COMMAND}

			# Only spawn so many processes at once
			COUNT=$((${COUNT}+1))
			if (( ${COUNT} % 30 == 0 )); then
				wait
			fi
		done
	done
done

# Wait for all background jobs to finish
wait

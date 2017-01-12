#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

# Kill background jobs if script is terminated
trap 'echo "Stopping..."; kill $(jobs -pr) 2>/dev/null; exit' SIGINT SIGTERM
# Remove Junk on exit
trap 'echo "Cleaning..."; rm -f calor.out; rm -f flukaerr.dat; rm -f gphysi.dat; exit' EXIT

# Run all simulations in "parallel" in background
COUNT=0
# excluded physics lists: "ShieldingLEND" "Shielding" "ShieldingM" "NuBeam" "FTFP_BERT_TRV" "QGSP_BIC_AllHP" "LBE"
for PLIST in "FTFP_BERT" "FTFP_BERT_HP" "FTFP_INCLXX" "FTFP_INCLXX_HP" "FTF_BIC" "QBBC" "QGSP_BERT" "QGSP_BERT_HP" "QGSP_BIC" "QGSP_BIC_HP" "QGSP_FTFP_BERT" "QGSP_INCLXX" "QGSP_INCLXX_HP"; do
    # Run over several orders of magnitude
	for b in $(seq 0 1 3); do for a in $(seq 1 1 9); do
		# Kinetic energy in MeV
		ENERGY=$(echo "$a*10^$b" | bc -l)
		# Note: The root call is extremely sensitive to the usage of ' and "
		COMMAND="Step1_Simulate.C(${ENERGY}, \"${OUTDIR}\", \"${PLIST}-${ENERGY}MeV\")"
		echo "$PLIST: $ENERGY"
		PHYSICSLIST=${PLIST} nice -n 19 root -l -q -b -e 'gInterpreter->AddIncludePath("'${VMCWORKDIR}'")' "${COMMAND}" &> "${OUTDIR}/${PLIST}-${ENERGY}MeV.log" &

		# Only spawn so many processes at once
		COUNT=$((${COUNT}+1))
		if (( ${COUNT} % 30 == 0 )); then
			wait
		fi
	done; done

	# Run over the area of interest with more detail
	for ENERGY in $(seq 50 100 950); do
		# Note: The root call is extremely sensitive to the usage of ' and "
		COMMAND="Step1_Simulate.C(${ENERGY}, \"${OUTDIR}\", \"${PLIST}-${ENERGY}MeV\")"
		echo "$PLIST: $ENERGY"
		PHYSICSLIST=${PLIST} nice -n 19 root -l -q -b -e 'gInterpreter->AddIncludePath("'${VMCWORKDIR}'")' "${COMMAND}" &> "${OUTDIR}/${PLIST}-${ENERGY}MeV.log" &

		# Only spawn so many processes at once
		COUNT=$((${COUNT}+1))
		if (( ${COUNT} % 30 == 0 )); then
			wait
		fi
	done
done

# Wait for all background jobs to finish
wait

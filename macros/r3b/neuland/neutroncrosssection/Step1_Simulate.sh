#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

NEVENTS=100000

# Kill background jobs if script is terminated
trap 'echo "Stopping..."; kill $(jobs -pr) 2>/dev/null; exit' SIGINT SIGTERM
# Remove Junk on exit
trap 'echo "Cleaning..."; rm -f calor.out; rm -f flukaerr.dat; rm -f gphysi.dat; exit' EXIT

# Run all simulations in "parallel" in background
COUNT=0
for PLIST in "FTFP_BERT" "FTFP_BERT_HP" "FTFP_INCLXX" "FTFP_INCLXX_HP" "FTF_BIC" "QBBC" "QGSP_BERT" "QGSP_BERT_HP" "QGSP_BIC" "QGSP_BIC_HP" "QGSP_FTFP_BERT" "QGSP_INCLXX" "QGSP_INCLXX_HP"; do
	# Not used: "ShieldingLEND" "Shielding" "ShieldingM" "NuBeam" "FTFP_BERT_TRV" "QGSP_BIC_AllHP" "LBE"
	for b in $(seq 0 1 3); do
		for a in $(seq 1 1 9); do
			# Kinetic energy in MeV
			ENERGY=$(echo "$a*10^$b" | bc -l)
			# Calculate momentum in GeV, p = √(K² + 2Kmc²)
			MOMENTUM=$(echo "sqrt((${ENERGY}^2)+(2*${ENERGY}*939.565379))/1000." | bc -l)

			# Note: The root call is extremely sensitive to the usage of ' and "
			COMMAND="Step1_Simulate.C(${MOMENTUM}, ${NEVENTS}, \"${OUTDIR}\", \"${PLIST}-${ENERGY}MeV\")"
			echo "$PLIST: $ENERGY -> $MOMENTUM --- ${COMMAND}"
			PHYSICSLIST=${PLIST} nice -n 19 root -l -q -b -e 'gInterpreter->AddIncludePath("'${VMCWORKDIR}'")' "${COMMAND}" &> "${OUTDIR}/${PLIST}-${ENERGY}MeV.log" &

			# Only spawn so many processes at once
			COUNT=$((${COUNT}+1))
			if (( ${COUNT} % 30 == 0 )); then
				wait
			fi
		done
	done
	for ENERGY in $(seq 50 100 950); do
		# Calculate momentum in GeV, p = √(K² + 2Kmc²)
		MOMENTUM=$(echo "sqrt((${ENERGY}^2)+(2*${ENERGY}*939.565379))/1000." | bc -l)

		# Note: The root call is extremely sensitive to the usage of ' and "
		COMMAND="Step1_Simulate.C(${MOMENTUM}, ${NEVENTS}, \"${OUTDIR}\", \"${PLIST}-${ENERGY}MeV\")"
		echo "$PLIST: $ENERGY -> $MOMENTUM --- ${COMMAND}"
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

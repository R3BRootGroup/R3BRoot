#!/bin/bash

# Check arguments
if [ -z "$1" ]; then
    echo "Usage: ./run.sh operation [output directory]"
    echo "Operations: simu digi reco eval"
    exit
fi
OPERATION=${1}

# Set output directory from script arguments or use default
OUTDIR=${2:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

# Kill background jobs if script is terminated
trap 'echo "Stopping..."; kill $(jobs -pr) 2>/dev/null; exit' SIGINT SIGTERM
# Remove Junk on exit
trap 'echo "Cleaning..."; rm -f calor.out; rm -f flukaerr.dat; rm -f gphysi.dat; rm -f core_dump*; exit' EXIT

# Look, I know this takes a lot of getting used to.
# - First, generate a list off all operations by expanding {l,i,s,t,s} or {r..anges}
# - Pipe those to xargs via echo -e
# - Use xargs as a process pool with up to --max-procs processes, it will start the next run once another finished
# - nice all heavyweight operation
# - use a metric ton of ' and " to make sure root gets the process in the right format
# - log output (enable extended bash regex via shopt -s extglob)

START=$(date +%s.%N)
#echo -e "'run.C(\"${OPERATION}\", \"${OUTDIR}\", 1400, "{300,600}", "{100,500}", "{1..50}", "{1..4}")'\n" | xargs -I CMD --max-procs 32 bash -c 'C='"'"'CMD'"'"'; echo `date --rfc-3339=seconds` ${C} && shopt -s extglob; nice -n 19 root -l -q -b -e "gInterpreter->AddIncludePath(\"'${VMCWORKDIR}'/macros/r3b/\")" "${C}" &> '${OUTDIR}'/${C//+([^a-zA-Z0-9])/_}.log'
echo -e "'run.C(\"${OPERATION}\", \"${OUTDIR}\", "{1400,3500}", "{300,600}", "{100,500}", "{12,30}", "{1..4}")'\n" | xargs -I CMD --max-procs 32 bash -c 'C='"'"'CMD'"'"'; echo `date --rfc-3339=seconds` ${C} && shopt -s extglob; nice -n 19 root -l -q -b -e "gInterpreter->AddIncludePath(\"'${VMCWORKDIR}'/macros/r3b/\")" "${C}" &> '${OUTDIR}'/${C//+([^a-zA-Z0-9])/_}.log'
END=$(date +%s.%N)

#"{1400,3500}"

# Echo duration
echo "$END - $START" | bc

#!/bin/bash

# This script performs a single thread task.
# $1 is the total number of threads used in the
# MT simulation and $2 is the current thread
# that applies to this single task.

# We only parallelize MC transport, 
# digitization, translation to signals
# and clustering.

# Test if the inputs are integers:
if [ $1 -eq $1 2>/dev/null -o $1 -eq 0 2>/dev/null ] && [ $2 -eq $2 2>/dev/null -o $2 -eq 0 2>/dev/null ]
then
    # Then, we have integers:
    if [ $1 -gt $2 ] && [ $1 -ge "0" ] && [ $2 -ge "0" ]
    # has to be, because $1 is nThreads and $2 is 
    # current thread ranging from 0 to n-1.
    then
        # NOTE: Then we can execute the task:
        cd ../Macros/
        root -l -q "MCtransport.cpp($1,$2)"
        root -l -q "NeuLAND_Digitizer.cpp($1,$2)"
        root -l -q "NeuLAND_JanClusters.cpp($1,$2)"
        root -l -q "NeuLAND_Translation.cpp($1,$2)"
        root -l -q "ClusterSignals.cpp($1,$2)"
        cd ../MT/
    else
        echo 'First input is nThreads and second is current thread from 0 to n-1!'
        echo 'Hence, the first input always has to be bigger then the second and both have to be zero or positive!'
        echo ''
    fi
else
    echo 'First input is nThreads and second is current thread from 0 to n-1!'
    echo 'Hence, both inputs have to be pure integers!'
    echo ''
fi

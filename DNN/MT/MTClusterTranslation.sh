#!/bin/bash

# This script performs a MultiThread Cluster translation
# It has only one input argument: the number of threads:

BOOLINPUT='kFALSE'
if [ "$2" == 'Step2' ]
then
BOOLINPUT='kTRUE'
fi

# Test if the input is an integer:
if [ $1 -eq $1 2>/dev/null -o $1 -eq 0 2>/dev/null ]
then
    # Test that it is a positive number:
    if [ $1 -gt "0" ]
    then
        # NOTE: Then we can execute the task. Begin by declaring an array 
        # of Process IDs: 
        declare -a PIDS[$1]
        
        # And fill it with zeros:
        for (( CurrentThread=0; CurrentThread<$1; ++CurrentThread ))
        do
            PIDS[${CurrentThread}]=0
        done
        
        # Loop over all processes:
        for (( CurrentThread=0; CurrentThread<$1; ++CurrentThread ))
        do
            # NOTE: It is the end-command that makes the processes run parallel:
            # So all but the last command should run with &:
            cd ../Macros
            root -l -q "TranslateClusters.cpp($1,${CurrentThread},${BOOLINPUT})" &
            cd ../MT
            
            # Store the background process PID:
            PIDS[${CurrentThread}]=$!
        done
        
        # Tell bash to quit processing after all background jobs are complete:
        for (( CurrentThread=0; CurrentThread<$1; ++CurrentThread ))
        do
            wait ${PIDS[${CurrentThread}]}
        done
        
        # Just to be sure:
        wait $!
        wait
        
        # Display all process IDs:
        for (( CurrentThread=0; CurrentThread<$1; ++CurrentThread ))
        do
            echo "Proces ID of Thread ${CurrentThread} = ${PIDS[${CurrentThread}]}"
        done
    else
        echo 'The input is the number of Threads, so it has to be positive!'
        echo ''
    fi
else
    echo 'The input is the number of Threads, so it has to be an integer!'
    echo ''
fi


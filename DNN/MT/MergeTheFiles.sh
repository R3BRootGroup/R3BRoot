#!/bin/bash

# This script merges the required files. It uses MT
# if the number of threads allow it. The input is the number
# of threads.

# Test if the input is an integer & positive:
if [ $1 -eq $1 2>/dev/null -o $1 -eq 0 2>/dev/null ] && [ $1 -gt "0" ]
then
    # for a single thread:
    if [ $1 -eq '1' ]
    then
        # single-thread merging:
        root -l -q "MergeFiles.cpp($1,0)"
        root -l -q "MergeFiles.cpp($1,1)"
        root -l -q "MergeFiles.cpp($1,2)"
        root -l -q "MergeFiles.cpp($1,3)"
    elif [ $1 -eq '2' ]
    then
        # double thread execution:
        root -l -q "MergeFiles.cpp($1,0)" &
        PID1=$!
        root -l -q "MergeFiles.cpp($1,1)" &
        PID2=$!
        wait ${PID1}
        wait ${PID2}
        
        root -l -q "MergeFiles.cpp($1,2)" &
        PID3=$!
        root -l -q "MergeFiles.cpp($1,3)" &
        PID4=$!
        wait ${PID3}
        wait ${PID4}
    elif [ $1 -eq '3' ]
    then
        # triple-thread execution:
        root -l -q "MergeFiles.cpp($1,1)" &
        PID1=$!
        root -l -q "MergeFiles.cpp($1,2)" &
        PID2=$!
        root -l -q "MergeFiles.cpp($1,3)" &
        PID3=$!
        wait ${PID1}
        wait ${PID2}
        wait ${PID3}
        
        root -l -q "MergeFiles.cpp($1,0)"
        PID4=$!
        wait ${PID4}
    else
        # multi-thread execution:
        root -l -q "MergeFiles.cpp($1,0)" &
        PID1=$!
        root -l -q "MergeFiles.cpp($1,1)" &
        PID2=$!
        root -l -q "MergeFiles.cpp($1,2)" &
        PID3=$!
        root -l -q "MergeFiles.cpp($1,3)" &
        PID4=$!
        wait ${PID1}
        wait ${PID2}
        wait ${PID3}
        wait ${PID4}
    fi
else
    echo 'The input is the number of Threads, so it has to be a positive integer!'
    echo ''
fi

# Also merge the histograms:
root -l -q "MergeHists.cpp($1,0)"
root -l -q "MergeHists.cpp($1,1)"
root -l -q "MergeHists.cpp($1,2)"
root -l -q "MergeHists.cpp($1,3)"

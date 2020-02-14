#!/bin/bash

# This script generates the Default ParFile for MT calculations
# from the ParFile of the first thread calculation. $1 is the total
# number of threads from the calculation.
wait

# Check that we have a positive number of threads:
if [ $1 -eq $1 2>/dev/null -o $1 -eq 0 2>/dev/null ] && [ $1 -gt "0" ]
then
    # Begin by generating the ParFile-filenames:
    root -l -q "GenerateFileNames.cpp($1,1)" # second arg. 1 states-->ParFile.

    # Then, read it:
    NLINES=`expr $1 + 1`
    declare -a NAMES[${NLINES}]
    COUNTER=0

    while read TheLine
    do
        NAMES[${COUNTER}]=${TheLine}
        COUNTER=`expr ${COUNTER} + 1`
    done < ./FileNames.txt
    
    # Next, copy the file. Use first thread (thread 0)
    # as default-file:
    cp ${NAMES[1]} ${NAMES[0]}
    
    # Done.
else
    echo 'You need to enter a strictly positive number!'
    echo ''
fi


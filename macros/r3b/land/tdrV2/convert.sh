#!/bin/bash

for i in *.eps; do
    gs -dSAFER -dBATCH -dNOPAUSE -dNOPROMPT -dMaxBitmap=500000000 -dEPSCrop -dAlignToPixels=0 -dGridFitTT=2 -sDEVICE=pngalpha -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -dUseCIEColor -r72x72 -sOUTPUTFILE="${i%.*}.png" ${i} &> /dev/null
done

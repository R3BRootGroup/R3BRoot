#!/bin/bash

OUT=/lustre/r3b/$(whoami)/c3w/logs/$1_*

cat $OUT
echo "that was the content of "
echo $OUT
echo "---- tail -f -------"
tail -f $OUT

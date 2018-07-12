#!/bin/sh

if [ 2 -ne $# ]
then
	echo "Usage: $0 Fi1a Fi2b"
	echo "This will rename files, classes, CMake and Root stuff from Fi1a to Fi2b."
	exit 1
fi

Src=$1
Dst=$2
src=$(echo $Src | tr '[:upper:]' '[:lower:]')
dst=$(echo $Dst | tr '[:upper:]' '[:lower:]')
SRC=$(echo $Src | tr '[:lower:]' '[:upper:]')
DST=$(echo $Dst | tr '[:lower:]' '[:upper:]')

if [ ! "$(ls | grep $Src)" ]
then
	echo "Wanted to rename from $Src, but I cannot find any files like that!"
	exit 1
fi
if [ "x$dst" != "x$(basename $(pwd))" ]
then
	echo "Wanted to rename to $Dst, but the current directory $(pwd) does not look like it!"
	exit 1
fi

for i in *
do
	sed -i "s/$src/$dst/g;s/$Src/$Dst/g;s/$SRC/$DST/g" $i
	mv $i $(echo $i | sed "s/$Src/$Dst/")
done

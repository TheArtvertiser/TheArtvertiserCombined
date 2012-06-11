#!/bin/sh

# call calcRoiAspectRatio.c
# requires calcRoiAspectRatio compiled: 
# gcc calcRoiAspectRation.c -o calcRoiAspectRatio

src=$1

# convert roi file to args for calcRoiAspectRatio app
roifile=$src
roi=`cat "$roifile" | while read x y; do
	echo $x $y
done`

./calcRoiAspectRatio $roi 320 240



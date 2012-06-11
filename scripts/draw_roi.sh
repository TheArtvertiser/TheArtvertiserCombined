#!/bin/sh

#convert /Users/damian/code/ofgit/apps/artvertiser-bhz/TheArtvertiserCombined/desktop/bin/data/adverts/20120609_153443660.bmp -fill none -stroke green -strokewidth 1 -draw "polygon 464,189 640,237 x,y x,y" out.png

src=$1
out=$2

# convert roi file to x,y path
roifile=$src.roi
rectcmd=`cat "$roifile" | while read x y; do
	echo $x,$y
done`
echo $rectcmd

convert "$src" -fill none -stroke green -strokewidth 2 -draw "polygon $rectcmd" "$out"



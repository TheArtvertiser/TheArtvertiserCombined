#!/bin/bahs

find . -iname "*.jpg" -or -iname "*.png" | while read f
do 
	identify -format "%w %h" "$f" | while read w h
	do 
		ratio=$(echo - | LC_ALL=c awk "{print $w/$h}")
		echo "$ratio $f"
	done
done


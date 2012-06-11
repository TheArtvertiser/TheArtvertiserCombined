find . -not -name "*.mov" -and -not -name "*.avi" | while read f; do echo $f; target="../participants-saturday-640480/$f"; if [ -f "$target" ]; then continue; else mkdir "`dirname "$target"`"; convert "$f" -resize 640x640 "$target"; fi; done


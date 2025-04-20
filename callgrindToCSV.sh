#!/bin/bash
# this is so I can more easily view the output of callgrind
echo "Instructions,Percentage,Location"

awk ' 
/^[[:space:]]*[0-9]/ {
	match($0, /^[[:space:]]*([0-9]+)/, icount)
	match($0, /\(\s*([0-9.]+)%\)/, pct)
	sub(/^.*%\)[[:space:]]*/, "", $0)
	printf "%s, %s, %s\n", icount[1], pct[1], $0
}
' "$1"

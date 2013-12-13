#!/bin/bash
# count code lines in current working directory
count=0
for file in *
do
	if [ -d $file ]; then
		echo "skip directory : $file"
		continue
	fi
	n=$(cat "$file" | wc -l)
	let "count += n"
done
echo "current code lines is $count"
exit 0
	

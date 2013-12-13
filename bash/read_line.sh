#!/bin/bash
line=initvalue
i=1
file_content=$(cat 1.txt)
while [ $line != "" ]
do
	read line < file_content
	echo "$i : $line"
	((i += 1))
done
echo "done"
exit 0

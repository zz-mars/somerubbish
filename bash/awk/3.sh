#!/bin/bash
if [ $# -ne 2 ] ;then
	echo "usage : comm + field + file"
	exit 1
fi
gawk -v "fields=$1" < $2 '
{
	count[$fields]++
}
END{
	for (x in count)
		printf "%-2d -- %s\n ",count[x],x
}'
exit 0

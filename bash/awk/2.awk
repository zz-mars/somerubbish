#!/bin/gawk -f
BEGIN{
	PLY="plymouth"
	CHV="chevorolet"
	OFS="\t"
}
{
	if ($1 ~ /ply/ ) $1=PLY
	if ($1 ~ /che/ ) $1=CHV
	print
}
END{
	print "end"
}

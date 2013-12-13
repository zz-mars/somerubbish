#!/bin/bash
TMPDIR=/tmp
if [ $# -ne 2 ] ;then
	echo "Usage file1 file2"
	exit 1
fi
file1=$TMPDIR/$$.file1
file2=$TMPDIR/$$.file2
sort "$1" > $file1
sort "$2" > $file2
exec 3<"$file1"
exec 4<"$file2"
read -u3 line1
s1=$?
read -u3 line2
s2=$?
while [ $s1 -eq 0 -a $s2 -eq 0 ]
do
	if [[ "$line1" < "$line2" ]] ;then
		echo -e "1\t$line1"
		read -u3 line1
		s1=$?
	else
		echo -e "2\t$line2"
		read -u4 line2
		s2=$?
	fi
done
while [ $s1 -eq 0 ]
do
	echo -e "1\t$line1"
	read -u3 line1
	s1=$?
done
while [ $s2 -eq 0 ]
do
	echo -e "2\t$line2"
	read -u4 line2
	s2=$?
done
exec 3<&- 4<&-
rm $file1 $file2
exit 0

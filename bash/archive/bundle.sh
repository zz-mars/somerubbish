#!/bin/bash
echo "# to unbundle ,bash this file"
if [ $# -eq 0 ] ;then
	set -- $(ls)
fi
echo "# arg -- $@"
for i
do
	if [ ! -d "$i" ] ;then
		echo "if [ \$# -eq 0 -o \"\$1\" = "$i" ] ;then"
		echo "echo $i 1>&2"
		echo "cat > $i <<'End of $i'"
		cat $i
		echo "End of $i"
		echo "fi"
	fi
done
exit 0

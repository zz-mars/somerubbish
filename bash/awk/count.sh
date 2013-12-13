#/bin/bash
if [ $# -ne 2 ] ;then
	echo "usage : "
	exit 1
fi
gawk < $2 '
		{count[$'$1']++}
END{
	for ( manu in count ) printf "%-20s\t%2d\n",manu,count[manu]
}' | sort -n

#!/bin/bash
echo -n "word 1:"
read word1
echo -n "word 2:"
read word2
echo -n "word 3:"
read word3
echo "Your input : "
echo "word1 : $word1"
echo "word2 : $word2"
echo "word3 : $word3"
if [ "$word1" = "$word2" -a "$word2" = "$word3" ] ;then
	echo "Match 1 2 3"
	elif [ "$word1" = "$word2" ] ;then
		echo "Match 1 2"
	elif [ "$word2" = "$word3" ] ;then
		echo "Match 2 3"
	elif [ "$word1" = "$word3" ] ;then
		echo "Match 1 3"
	else echo "NO MATCH"
fi
echo "End of program"
exit 0

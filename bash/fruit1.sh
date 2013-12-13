#!/bin/bash
PS3="choose your favourite fruit : "
select fruit in apple banana blueberry kiwi orange watermelon STOP
do
	if [ "$fruit" = "" ] ;then
		echo "Invalid choice"
		continue
	elif [ "$fruit" = STOP ] ;then
		echo "thanks for playing!"
		break
	fi
	echo "you choose $fruit as your favourite fruit!"
	echo "num you choose is $REPLY"
done


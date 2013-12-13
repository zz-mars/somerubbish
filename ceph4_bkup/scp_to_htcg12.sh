#!/bin/bash
dst_dir=/media/532B-5105/wandoujia/music
for arg
do
	scp "$arg" zz.chen:$dst_dir/
done
exit 0

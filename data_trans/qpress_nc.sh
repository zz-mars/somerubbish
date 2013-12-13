#!/bin/bash
s243="root@192.168.0.243"
#scp /usr/local/bin/qpress $s243 :/usr/local/bin/qpress
ssh $s243 "nc -l 12340 | qpress -dio > /var/www/html/planetearth03.mkv" &
	qpress -o /media/F/Planet\ Earth/Planet.Earth.EP01.2006.BDRip.a720.2Audio.X264.MiniHD-TLF.mkv | nc $s243 12340
exit 0

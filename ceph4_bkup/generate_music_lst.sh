#!/bin/bash
music_dir=/var/www/html/music/
music_lst=/var/www/html/index.html
echo > "$music_lst"
echo "<html>                                                                             " >> "$music_lst"
echo "<title>zz\'s music list</title>                                                     " >> "$music_lst"
echo "<body>                                                                             " >> "$music_lst"
echo "<p>-------------------for microsoft employ interview ---------------------</p></br>" >> "$music_lst"
echo "<a href=\"/AttendeeSetupUser.exe\">the fucking lync software</a></br>                " >> "$music_lst"
echo "<p>-------------------Album---------------------</p></br>                          " >> "$music_lst"
echo "<a href=\"/Fedora-15-i386-DVD.iso\">Fedora_15</a></br>                               " >> "$music_lst"
echo "<a href=\"/ubuntu-13.04-desktop-amd64.iso\">ubuntu 13.04-amd64.iso</a></br>          " >> "$music_lst"
echo "                                                                                   " >> "$music_lst"
echo "<a href=\"music/suede_coming_up/\">Suede .-. Coming Up</a></br>                      " >> "$music_lst"
echo "<a href=\"music/whatsthestory/\">Oasis .-. What's the story?(Morning Glory)</a></br> " >> "$music_lst"
echo "<p>-------------------singles---------------------</p></br>                         " >> "$music_lst"
cd "$music_dir"
for file in *.mp3
do
	echo "<a href=\"/music/$file\">$file</a></br>" >> "$music_lst"
done
echo "</body>" >> "$music_lst"
echo "</html>" >> "$music_lst"
exit 0

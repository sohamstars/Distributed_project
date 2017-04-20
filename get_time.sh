#!bin/bash

multichain-cli chain1 liststreamkeyitems localstream $1 | grep 'key\|data\|blocktime' >time.txt && sed -i 's/ //g' time.txt

#tac time.txt | grep '00000000' -m 1 -B 1 | tac | grep blocktime>time1.txt

sed '/blocktime/h;g;$!d' time.txt > time1.txt

echo "executed shell script"


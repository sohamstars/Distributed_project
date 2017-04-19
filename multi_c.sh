#!bin/bash

multichain-cli chain1 liststreamitems localstream | grep 'key\|data' > sd.txt && sed -i 's/ //g' sd.txt
echo "executed shell script"

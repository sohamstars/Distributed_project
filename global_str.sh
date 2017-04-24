#!bin/bash

multichain-cli chain1 liststreamkeyitems $1 $2 | grep 'key\|data' > sd1.txt && sed -i 's/ //g' sd.txt
echo "executed shell script in global stream"

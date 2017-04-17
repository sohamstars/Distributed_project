#!bin/bash

multichain-cli new2 liststreamkeyitems globalstream $1 | grep 'key\|data' > sd1.txt && sed -i 's/ //g' sd.txt
echo "executed shell script in global stream"
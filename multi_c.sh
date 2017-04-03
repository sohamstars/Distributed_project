#!bin/bash

multichain-cli chan1 liststreamitems stream1 | grep 'key\|data' > sd.txt && sed -i 's/ //g' sd.txt
echo "executed shell script"
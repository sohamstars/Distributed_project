#!bin/bash

multichain-cli new2 liststreamitems local_Anish | grep 'key\|data' > sd.txt && sed -i 's/ //g' sd.txt
echo "executed shell script"
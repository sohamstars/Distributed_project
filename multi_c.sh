#!bin/bash

multichain-cli chain1 liststreamitems localstream true 65535 0| grep 'key\|data' > sd.txt && sed -i 's/ //g' sd.txt
echo "executed shell script"

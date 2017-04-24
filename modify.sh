tac sd1.txt | grep '00000000' -m 1 -B 9999 | grep data > out.txt
sed 's/\"data\" : \"//g' out.txt  > out1.txt
sed 's/        //g' out1.txt > out2.txt
sed 's/\",//g' out2.txt > out3.txt

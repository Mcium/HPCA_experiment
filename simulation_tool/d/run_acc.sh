NET_TYPE="pv"
SCHEME="a"
ENG_SRS="piezo"
#mkdir "exec_"$NET_TYPE
#cd "exec_"$NET_TYPE
#cp -rf ../accelerator .


echo "Starting Exec"

printf "thermal y d lenet" | ./accelerator
printf "thermal y d pv" | ./accelerator
printf "thermal y d fr" | ./accelerator
printf "thermal y d hg" | ./accelerator
echo "Done Exec solar"

printf "piezo y d lenet" | ./accelerator
printf "piezo y d pv" | ./accelerator
printf "piezo y d fr" | ./accelerator
printf "piezo y d hg" | ./accelerator
echo "Done Exec piezo"



echo "Ececution Done for a"

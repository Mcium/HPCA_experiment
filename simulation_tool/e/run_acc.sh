NET_TYPE="pv"
SCHEME="a"
ENG_SRS="piezo"
#mkdir "exec_"$NET_TYPE
#cd "exec_"$NET_TYPE
#cp -rf ../accelerator .


echo "Starting Exec"

printf "thermal y lenet" | ./accelerator
printf "thermal y pv" | ./accelerator
printf "thermal y fr" | ./accelerator
printf "thermal y hg" | ./accelerator
echo "Done Exec solar"

printf "piezo y lenet" | ./accelerator
printf "piezo y pv" | ./accelerator
printf "piezo y fr" | ./accelerator
printf "piezo y hg" | ./accelerator
echo "Done Exec piezo"



echo "Ececution Done for a"

NET_TYPE="pv"
SCHEME="a"
ENG_SRS="piezo"
#mkdir "exec_"$NET_TYPE
#cd "exec_"$NET_TYPE
#cp -rf ../accelerator .


echo "Starting Exec"

printf "thermal y c lenet" | ./accelerator
printf "thermal y c pv" | ./accelerator
printf "thermal y c fr" | ./accelerator
printf "thermal y c hg" | ./accelerator
echo "Done Exec solar"

printf "piezo y c lenet" | ./accelerator
printf "piezo y c pv" | ./accelerator
printf "piezo y c fr" | ./accelerator
printf "piezo y c hg" | ./accelerator
echo "Done Exec piezo"



echo "Ececution Done for a"

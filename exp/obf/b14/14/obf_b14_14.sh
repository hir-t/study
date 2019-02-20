# Circuit : b14
# Loops	  : 1
# length  : 4
script b14res_obf14.txt time ./decrypto.sh b14_C.bench 1 4
# "CPU time"の行を取り出す
cat b14res_obf14.txt | grep CPU > b14time_obf14.txt
exit


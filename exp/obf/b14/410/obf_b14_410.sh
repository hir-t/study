# Circuit : b14
# Loops	  : 4
# length  : 10
script b14res_obf410.txt time ./decrypto.sh b14_C.bench 4 10
# "CPU time"の行を取り出す
cat b14res_obf410.txt | grep CPU > b14time_obf410.txt
exit


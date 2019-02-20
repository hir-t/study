# Circuit : b14
# Loops	  : 6
# length  : 4
script b14res_obf64.txt time ./decrypto.sh b14_C.bench 6 4
# "CPU time"の行を取り出す
cat b14res_obf64.txt | grep CPU > b14time_obf64.txt
exit


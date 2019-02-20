# Circuit : b14
# Loops	  : 1
# length  : 8
script b14res_obf18.txt time ./decrypto.sh b14_C.bench 1 8
# "CPU time"の行を取り出す
cat b14res_obf18.txt | grep CPU > b14time_obf18.txt
exit


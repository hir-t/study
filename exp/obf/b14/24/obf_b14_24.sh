# Circuit : b14
# Loops	  : 2
# length  : 4
script b14res_obf24.txt time ./decrypto.sh b14_C.bench 2 4
# "CPU time"の行を取り出す
cat b14res_obf24.txt | grep CPU > b14time_obf24.txt
exit


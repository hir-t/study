# Circuit : b14
# Loops	  : 3
# length  : 6
script b14res_obf36.txt time ./decrypto.sh b14_C.bench 3 6
# "CPU time"の行を取り出す
cat b14res_obf36.txt | grep CPU > b14time_obf36.txt
exit


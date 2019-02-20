# Circuit : b14
# Loops	  : 3
# length  : 8
script b14res_obf38.txt time ./decrypto.sh b14_C.bench 3 8
# "CPU time"の行を取り出す
cat b14res_obf38.txt | grep CPU > b14time_obf38.txt
exit


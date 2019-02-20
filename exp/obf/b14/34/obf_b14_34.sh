# Circuit : b14
# Loops	  : 3
# length  : 4
script b14res_obf34.txt time ./decrypto.sh b14_C.bench 3 4
# "CPU time"の行を取り出す
cat b14res_obf34.txt | grep CPU > b14time_obf34.txt
exit


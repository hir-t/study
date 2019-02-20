# Circuit : b14
# Loops	  : 2
# length  : 8
script b14res_obf28.txt time ./decrypto.sh b14_C.bench 2 8
# "CPU time"の行を取り出す
cat b14res_obf28.txt | grep CPU > b14time_obf28.txt
exit


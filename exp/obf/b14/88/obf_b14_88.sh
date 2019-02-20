# Circuit : b14
# Loops	  : 8
# length  : 8
script b14res_obf88.txt time ./decrypto.sh b14_C.bench 8 8
# "CPU time"の行を取り出す
cat b14res_obf88.txt | grep CPU > b14time_obf88.txt
exit


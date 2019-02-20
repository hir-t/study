# Circuit : b14
# Loops	  : 8
# length  : 10
script b14res_obf810.txt time ./decrypto.sh b14_C.bench 8 10
# "CPU time"の行を取り出す
cat b14res_obf810.txt | grep CPU > b14time_obf810.txt
exit


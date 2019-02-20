# Circuit : b14
# Loops	  : 2
# length  : 10
script b14res_obf210.txt time ./decrypto.sh b14_C.bench 2 10
# "CPU time"の行を取り出す
cat b14res_obf210.txt | grep CPU > b14time_obf210.txt
exit


# Circuit : b14
# Loops	  : 3
# length  : 10
script b14res_obf310.txt time ./decrypto.sh b14_C.bench 3 10
# "CPU time"の行を取り出す
cat b14res_obf310.txt | grep CPU > b14time_obf310.txt
exit


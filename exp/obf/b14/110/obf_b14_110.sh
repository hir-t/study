# Circuit : b14
# Loops	  : 1
# length  : 10
script b14res_obf110.txt time ./decrypto.sh b14_C.bench 1 10
# "CPU time"の行を取り出す
cat b14res_obf110.txt | grep CPU > b14time_obf110.txt
exit


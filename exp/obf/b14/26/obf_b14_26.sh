# Circuit : b14
# Loops	  : 2
# length  : 6
script b14res_obf26.txt time ./decrypto.sh b14_C.bench 2 6
# "CPU time"の行を取り出す
cat b14res_obf26.txt | grep CPU > b14time_obf26.txt
exit


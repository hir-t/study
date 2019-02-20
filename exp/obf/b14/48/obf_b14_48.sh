# Circuit : b14
# Loops	  : 4
# length  : 8
script b14res_obf48.txt time ./decrypto.sh b14_C.bench 4 8
# "CPU time"の行を取り出す
cat b14res_obf48.txt | grep CPU > b14time_obf48.txt
exit


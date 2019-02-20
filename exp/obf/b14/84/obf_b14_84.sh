# Circuit : b14
# Loops	  : 8
# length  : 4
script b14res_obf84.txt time ./decrypto.sh b14_C.bench 8 4
# "CPU time"の行を取り出す
cat b14res_obf84.txt | grep CPU > b14time_obf84.txt
exit


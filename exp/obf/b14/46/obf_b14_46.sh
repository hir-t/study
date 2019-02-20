# Circuit : b14
# Loops	  : 4
# length  : 6
script b14res_obf46.txt time ./decrypto.sh b14_C.bench 4 6
# "CPU time"の行を取り出す
cat b14res_obf46.txt | grep CPU > b14time_obf46.txt
exit


# Circuit : b14
# Loops	  : 6
# length  : 8
script b14res_obf68.txt time ./decrypto.sh b14_C.bench 6 8
# "CPU time"の行を取り出す
cat b14res_obf68.txt | grep CPU > b14time_obf68.txt
exit


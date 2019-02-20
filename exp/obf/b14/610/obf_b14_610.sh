# Circuit : b14
# Loops	  : 6
# length  : 10
script b14res_obf610.txt time ./decrypto.sh b14_C.bench 6 10
# "CPU time"の行を取り出す
cat b14res_obf610.txt | grep CPU > b14time_obf610.txt
exit


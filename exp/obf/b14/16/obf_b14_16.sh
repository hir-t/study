# Circuit : b14
# Loops	  : 1
# length  : 6
script b14res_obf16.txt time ./decrypto.sh b14_C.bench 1 6
# "CPU time"の行を取り出す
cat b14res_obf16.txt | grep CPU > b14time_obf16.txt
exit


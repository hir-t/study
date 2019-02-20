# decryoto
# Circuit : b14
# Loops	  : 8
# length  : 3
script b14res_obf83.txt time ./decrypto.sh b14_C.bench 8 3
# "CPU time"の行を取り出す
cat b14res_obf83.txt | grep CPU > b14time_obf83.txt
exit


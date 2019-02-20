# decryoto
# Circuit : b14
# Loops	  : 4
# length  : 3
script b14res_obf43.txt time ./decrypto.sh b14_C.bench 4 3
# "CPU time"の行を取り出す
cat b14res_obf43.txt | grep CPU > b14time_obf43.txt
exit


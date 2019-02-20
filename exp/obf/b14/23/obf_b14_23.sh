# decryoto
# Circuit : b14
# Loops	  : 2
# length  : 3
script b14res_obf23.txt time ./decrypto.sh b14_C.bench 2 3
# "CPU time"の行を取り出す
cat b14res_obf23.txt | grep CPU > b14time_obf23.txt
exit


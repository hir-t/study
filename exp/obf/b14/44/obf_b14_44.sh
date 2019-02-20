# decryoto
# Circuit : b14
# Loops	  : 4
# length  : 4
script b14res_obf44.txt time ./decrypto.sh b14_C.bench 4 4
# "CPU time"の行を取り出す
cat b14res_obf44.txt | grep CPU > b14time_obf44.txt
exit


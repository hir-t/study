# decryoto
# Circuit : b14
# Loops	  : 6
# length  : 3
script b14res_obf63.txt time ./decrypto.sh b14_C.bench 6 3
# "CPU time"の行を取り出す
cat b14res_obf63.txt | grep CPU > b14time_obf63.txt
exit


# decryoto
# Circuit : b14
# Loops	  : 3
# length  : 3
script b14res_obf33.txt time ./decrypto.sh b14_C.bench 3 3
# "CPU time"の行を取り出す
cat b14res_obf33.txt | grep CPU > b14time_obf33.txt
exit


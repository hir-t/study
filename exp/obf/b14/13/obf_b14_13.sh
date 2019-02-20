# decryoto
# Circuit : b14
# Loops	  : 1
# length  : 3
script b14res_obf13.txt time ./decrypto.sh b14_C.bench 1 3
# "CPU time"の行を取り出す
cat b14res_obf13.txt | grep CPU > b14time_obf13.txt
exit


# Circuit : b14
# Loops	  : 8
# length  : 6
script b14res_obf86.txt time ./decrypto.sh b14_C.bench 8 6
# "CPU time"の行を取り出す
cat b14res_obf86.txt | grep CPU > b14time_obf86.txt
exit


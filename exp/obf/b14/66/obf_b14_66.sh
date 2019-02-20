# Circuit : b14
# Loops	  : 6
# length  : 6
script b14res_obf66.txt time ./decrypto.sh b14_C.bench 6 6
# "CPU time"の行を取り出す
cat b14res_obf66.txt | grep CPU > b14time_obf66.txt
exit


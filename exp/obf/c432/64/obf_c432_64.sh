# Circuit : c432
# Loops	  : 6
# length  : 4
script c432res_obf64.txt time ./decrypto.sh c432.bench 6 4
# "CPU time"の行を取り出す
cat c432res_obf64.txt | grep CPU > c432time_obf64.txt
exit


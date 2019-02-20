# Circuit : c432
# Loops	  : 1
# length  : 4
script c432res_obf14.txt time ./decrypto.sh c432.bench 1 4
# "CPU time"の行を取り出す
cat c432res_obf14.txt | grep CPU > c432time_obf14.txt
exit


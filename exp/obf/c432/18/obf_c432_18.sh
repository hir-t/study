# Circuit : c432
# Loops	  : 1
# length  : 8
script c432res_obf18.txt time ./decrypto.sh c432.bench 1 8
# "CPU time"の行を取り出す
cat c432res_obf18.txt | grep CPU > c432time_obf18.txt
exit


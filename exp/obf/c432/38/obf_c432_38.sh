# Circuit : c432
# Loops	  : 3
# length  : 8
script c432res_obf38.txt time ./decrypto.sh c432.bench 3 8
# "CPU time"の行を取り出す
cat c432res_obf38.txt | grep CPU > c432time_obf38.txt
exit


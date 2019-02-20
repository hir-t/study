# Circuit : c432
# Loops	  : 6
# length  : 8
script c432res_obf68.txt time ./decrypto.sh c432.bench 6 8
# "CPU time"の行を取り出す
cat c432res_obf68.txt | grep CPU > c432time_obf68.txt
exit


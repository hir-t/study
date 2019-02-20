# Circuit : c432
# Loops	  : 4
# length  : 8
script c432res_obf48.txt time ./decrypto.sh c432.bench 4 8
# "CPU time"の行を取り出す
cat c432res_obf48.txt | grep CPU > c432time_obf48.txt
exit


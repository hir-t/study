# Circuit : c432
# Loops	  : 8
# length  : 8
script c432res_obf88.txt time ./decrypto.sh c432.bench 8 8
# "CPU time"の行を取り出す
cat c432res_obf88.txt | grep CPU > c432time_obf88.txt
exit


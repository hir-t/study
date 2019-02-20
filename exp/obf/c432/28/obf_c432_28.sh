# Circuit : c432
# Loops	  : 2
# length  : 8
script c432res_obf28.txt time ./decrypto.sh c432.bench 2 8
# "CPU time"の行を取り出す
cat c432res_obf28.txt | grep CPU > c432time_obf28.txt
exit


# Circuit : c432
# Loops	  : 3
# length  : 6
script c432res_obf36.txt time ./decrypto.sh c432.bench 3 6
# "CPU time"の行を取り出す
cat c432res_obf36.txt | grep CPU > c432time_obf36.txt
exit


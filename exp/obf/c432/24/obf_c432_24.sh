# Circuit : c432
# Loops	  : 2
# length  : 4
script c432res_obf24.txt time ./decrypto.sh c432.bench 2 4
# "CPU time"の行を取り出す
cat c432res_obf24.txt | grep CPU > c432time_obf24.txt
exit


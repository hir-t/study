# Circuit : c432
# Loops	  : 2
# length  : 6
script c432res_obf26.txt time ./decrypto.sh c432.bench 2 6
# "CPU time"の行を取り出す
cat c432res_obf26.txt | grep CPU > c432time_obf26.txt
exit


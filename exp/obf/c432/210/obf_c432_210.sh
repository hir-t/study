# Circuit : c432
# Loops	  : 2
# length  : 10
script c432res_obf210.txt time ./decrypto.sh c432.bench 2 10
# "CPU time"の行を取り出す
cat c432res_obf210.txt | grep CPU > c432time_obf210.txt
exit


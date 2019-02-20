# Circuit : c432
# Loops	  : 4
# length  : 10
script c432res_obf410.txt time ./decrypto.sh c432.bench 4 10
# "CPU time"の行を取り出す
cat c432res_obf410.txt | grep CPU > c432time_obf410.txt
exit


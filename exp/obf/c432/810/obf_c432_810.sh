# Circuit : c432
# Loops	  : 8
# length  : 10
script c432res_obf810.txt time ./decrypto.sh c432.bench 8 10
# "CPU time"の行を取り出す
cat c432res_obf810.txt | grep CPU > c432time_obf810.txt
exit


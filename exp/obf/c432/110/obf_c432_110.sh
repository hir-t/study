# Circuit : c432
# Loops	  : 1
# length  : 10
script c432res_obf110.txt time ./decrypto.sh c432.bench 1 10
# "CPU time"の行を取り出す
cat c432res_obf110.txt | grep CPU > c432time_obf110.txt
exit


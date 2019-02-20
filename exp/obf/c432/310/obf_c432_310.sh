# Circuit : c432
# Loops	  : 3
# length  : 10
script c432res_obf310.txt time ./decrypto.sh c432.bench 3 10
# "CPU time"の行を取り出す
cat c432res_obf310.txt | grep CPU > c432time_obf310.txt
exit


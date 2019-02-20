# Circuit : c432
# Loops	  : 3
# length  : 4
script c432res_obf34.txt time ./decrypto.sh c432.bench 3 4
# "CPU time"の行を取り出す
cat c432res_obf34.txt | grep CPU > c432time_obf34.txt
exit


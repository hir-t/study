# Circuit : c432
# Loops	  : 4
# length  : 6
script c432res_obf46.txt time ./decrypto.sh c432.bench 4 6
# "CPU time"の行を取り出す
cat c432res_obf46.txt | grep CPU > c432time_obf46.txt
exit

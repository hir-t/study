# Circuit : c432
# Loops	  : 8
# length  : 4
script c432res_obf84.txt time ./decrypto.sh c432.bench 8 4
# "CPU time"の行を取り出す
cat c432res_obf84.txt | grep CPU > c432time_obf84.txt
exit


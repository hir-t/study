# decryoto
# Circuit : c432
# Loops	  : 4
# length  : 4
script c432res_obf44.txt time ./decrypto.sh c432.bench 4 4
# "CPU time"の行を取り出す
cat c432res_obf44.txt | grep CPU > c432time_obf44.txt
exit


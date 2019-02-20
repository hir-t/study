# decryoto
# Circuit : c432
# Loops	  : 2
# length  : 3
script c432res_obf23.txt time ./decrypto.sh c432.bench 2 3
# "CPU time"の行を取り出す
cat c432res_obf23.txt | grep CPU > c432time_obf23.txt
exit


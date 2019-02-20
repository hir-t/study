# decryoto
# Circuit : c432
# Loops	  : 6
# length  : 3
script c432res_obf63.txt time ./decrypto.sh c432.bench 6 3
# "CPU time"の行を取り出す
cat c432res_obf63.txt | grep CPU > c432time_obf63.txt
exit


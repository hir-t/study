# decryoto
# Circuit : c432
# Loops	  : 8
# length  : 3
script c432res_obf83.txt time ./decrypto.sh c432.bench 8 3
# "CPU time"の行を取り出す
cat c432res_obf83.txt | grep CPU > c432time_obf83.txt
exit


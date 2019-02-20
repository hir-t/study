# decryoto
# Circuit : c432
# Loops	  : 3
# length  : 3
script c432res_obf33.txt time ./decrypto.sh c432.bench 3 3
# "CPU time"の行を取り出す
cat c432res_obf33.txt | grep CPU > c432time_obf33.txt
exit


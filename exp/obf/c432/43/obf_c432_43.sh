# decryoto
# Circuit : c432
# Loops	  : 4
# length  : 3
script c432res_obf43.txt time ./decrypto.sh c432.bench 4 3
# "CPU time"の行を取り出す
cat c432res_obf43.txt | grep CPU > c432time_obf43.txt
exit


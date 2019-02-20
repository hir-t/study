# Circuit : c432
# Loops	  : 8
# length  : 6
script c432res_obf86.txt time ./decrypto.sh c432.bench 8 6
# "CPU time"の行を取り出す
cat c432res_obf86.txt | grep CPU > c432time_obf86.txt
exit


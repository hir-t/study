# Circuit : c432
# Loops	  : 6
# length  : 10
script c432res_obf610.txt time ./decrypto.sh c432.bench 6 10
# "CPU time"の行を取り出す
cat c432res_obf610.txt | grep CPU > c432time_obf610.txt
exit


# Circuit : c432
# Loops	  : 1
# length  : 6
script c432res_obf16.txt time ./decrypto.sh c432.bench 1 6
# "CPU time"の行を取り出す
cat c432res_obf16.txt | grep CPU > c432time_obf16.txt
exit


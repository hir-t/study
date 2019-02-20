# Circuit : c432
# Loops	  : 6
# length  : 6
script c432res_obf66.txt time ./decrypto.sh c432.bench 6 6
# "CPU time"の行を取り出す
cat c432res_obf66.txt | grep CPU > c432time_obf66.txt
exit


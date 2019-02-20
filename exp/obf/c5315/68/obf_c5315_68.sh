# Circuit : c5315
# Loops	  : 6
# length  : 8
script c5315res_obf68.txt time ./decrypto.sh c5315.bench 6 8
# "CPU time"の行を取り出す
cat c5315res_obf68.txt | grep CPU > c5315time_obf68.txt
exit


# Circuit : c5315
# Loops	  : 6
# length  : 4
script c5315res_obf64.txt time ./decrypto.sh c5315.bench 6 4
# "CPU time"の行を取り出す
cat c5315res_obf64.txt | grep CPU > c5315time_obf64.txt
exit


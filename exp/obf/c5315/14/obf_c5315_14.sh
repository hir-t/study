# Circuit : c5315
# Loops	  : 1
# length  : 4
script c5315res_obf14.txt time ./decrypto.sh c5315.bench 1 4
# "CPU time"の行を取り出す
cat c5315res_obf14.txt | grep CPU > c5315time_obf14.txt
exit


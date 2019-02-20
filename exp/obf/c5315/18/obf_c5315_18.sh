# Circuit : c5315
# Loops	  : 1
# length  : 8
script c5315res_obf18.txt time ./decrypto.sh c5315.bench 1 8
# "CPU time"の行を取り出す
cat c5315res_obf18.txt | grep CPU > c5315time_obf18.txt
exit


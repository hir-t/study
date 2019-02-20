# Circuit : c5315
# Loops	  : 3
# length  : 6
script c5315res_obf36.txt time ./decrypto.sh c5315.bench 3 6
# "CPU time"の行を取り出す
cat c5315res_obf36.txt | grep CPU > c5315time_obf36.txt
exit


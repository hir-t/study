# Circuit : c5315
# Loops	  : 4
# length  : 8
script c5315res_obf48.txt time ./decrypto.sh c5315.bench 4 8
# "CPU time"の行を取り出す
cat c5315res_obf48.txt | grep CPU > c5315time_obf48.txt
exit


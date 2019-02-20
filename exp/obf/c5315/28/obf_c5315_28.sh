# Circuit : c5315
# Loops	  : 2
# length  : 8
script c5315res_obf28.txt time ./decrypto.sh c5315.bench 2 8
# "CPU time"の行を取り出す
cat c5315res_obf28.txt | grep CPU > c5315time_obf28.txt
exit


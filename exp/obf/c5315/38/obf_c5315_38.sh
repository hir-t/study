# Circuit : c5315
# Loops	  : 3
# length  : 8
script c5315res_obf38.txt time ./decrypto.sh c5315.bench 3 8
# "CPU time"の行を取り出す
cat c5315res_obf38.txt | grep CPU > c5315time_obf38.txt
exit


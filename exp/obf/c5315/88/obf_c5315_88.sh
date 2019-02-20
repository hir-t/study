# Circuit : c5315
# Loops	  : 8
# length  : 8
script c5315res_obf88.txt time ./decrypto.sh c5315.bench 8 8
# "CPU time"の行を取り出す
cat c5315res_obf88.txt | grep CPU > c5315time_obf88.txt
exit


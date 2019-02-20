# Circuit : c5315
# Loops	  : 3
# length  : 4
script c5315res_obf34.txt time ./decrypto.sh c5315.bench 3 4
# "CPU time"の行を取り出す
cat c5315res_obf34.txt | grep CPU > c5315time_obf34.txt
exit


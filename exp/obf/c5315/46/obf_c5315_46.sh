# Circuit : c5315
# Loops	  : 4
# length  : 6
script c5315res_obf46.txt time ./decrypto.sh c5315.bench 4 6
# "CPU time"の行を取り出す
cat c5315res_obf46.txt | grep CPU > c5315time_obf46.txt
exit


# Circuit : c5315
# Loops	  : 2
# length  : 4
script c5315res_obf24.txt time ./decrypto.sh c5315.bench 2 4
# "CPU time"の行を取り出す
cat c5315res_obf24.txt | grep CPU > c5315time_obf24.txt
exit


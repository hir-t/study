# Circuit : c5315
# Loops	  : 2
# length  : 6
script c5315res_obf26.txt time ./decrypto.sh c5315.bench 2 6
# "CPU time"の行を取り出す
cat c5315res_obf26.txt | grep CPU > c5315time_obf26.txt
exit


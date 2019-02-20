# Circuit : c5315
# Loops	  : 2
# length  : 10
script c5315res_obf210.txt time ./decrypto.sh c5315.bench 2 10
# "CPU time"の行を取り出す
cat c5315res_obf210.txt | grep CPU > c5315time_obf210.txt
exit


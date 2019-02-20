# Circuit : c5315
# Loops	  : 4
# length  : 10
script c5315res_obf410.txt time ./decrypto.sh c5315.bench 4 10
# "CPU time"の行を取り出す
cat c5315res_obf410.txt | grep CPU > c5315time_obf410.txt
exit


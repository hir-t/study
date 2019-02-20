# Circuit : c5315
# Loops	  : 3
# length  : 10
script c5315res_obf310.txt time ./decrypto.sh c5315.bench 3 10
# "CPU time"の行を取り出す
cat c5315res_obf310.txt | grep CPU > c5315time_obf310.txt
exit


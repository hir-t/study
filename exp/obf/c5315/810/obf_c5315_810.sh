# Circuit : c5315
# Loops	  : 8
# length  : 10
script c5315res_obf810.txt time ./decrypto.sh c5315.bench 8 10
# "CPU time"の行を取り出す
cat c5315res_obf810.txt | grep CPU > c5315time_obf810.txt
exit


# decryoto
# Circuit : c5315
# Loops	  : 2
# length  : 3
script c5315res_obf23.txt time ./decrypto.sh c5315.bench 2 3
# "CPU time"の行を取り出す
cat c5315res_obf23.txt | grep CPU > c5315time_obf23.txt
exit


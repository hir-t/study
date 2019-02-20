# decryoto
# Circuit : c5315
# Loops	  : 4
# length  : 4
script c5315res_obf44.txt time ./decrypto.sh c5315.bench 4 4
# "CPU time"の行を取り出す
cat c5315res_obf44.txt | grep CPU > c5315time_obf44.txt
exit


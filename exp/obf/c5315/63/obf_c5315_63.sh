# decryoto
# Circuit : c5315
# Loops	  : 6
# length  : 3
script c5315res_obf63.txt time ./decrypto.sh c5315.bench 6 3
# "CPU time"の行を取り出す
cat c5315res_obf63.txt | grep CPU > c5315time_obf63.txt
exit


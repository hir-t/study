# decryoto
# Circuit : c5315
# Loops	  : 3
# length  : 3
script c5315res_obf33.txt time ./decrypto.sh c5315.bench 3 3
# "CPU time"の行を取り出す
cat c5315res_obf33.txt | grep CPU > c5315time_obf33.txt
exit


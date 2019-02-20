# decryoto
# Circuit : c5315
# Loops	  : 4
# length  : 3
script c5315res_obf43.txt time ./decrypto.sh c5315.bench 4 3
# "CPU time"の行を取り出す
cat c5315res_obf43.txt | grep CPU > c5315time_obf43.txt
exit


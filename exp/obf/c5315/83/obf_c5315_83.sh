# decryoto
# Circuit : c5315
# Loops	  : 8
# length  : 3
script c5315res_obf83.txt time ./decrypto.sh c5315.bench 8 3
# "CPU time"の行を取り出す
cat c5315res_obf83.txt | grep CPU > c5315time_obf83.txt
exit


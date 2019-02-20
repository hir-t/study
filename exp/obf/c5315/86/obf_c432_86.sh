# Circuit : c5315
# Loops	  : 8
# length  : 6
script c5315res_obf86.txt time ./decrypto.sh c5315.bench 8 6
# "CPU time"の行を取り出す
cat c5315res_obf86.txt | grep CPU > c5315time_obf86.txt
exit


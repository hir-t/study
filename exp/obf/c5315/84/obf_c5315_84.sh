# Circuit : c5315
# Loops	  : 8
# length  : 4
script c5315res_obf84.txt time ./decrypto.sh c5315.bench 8 4
# "CPU time"の行を取り出す
cat c5315res_obf84.txt | grep CPU > c5315time_obf84.txt
exit


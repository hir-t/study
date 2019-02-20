# decryoto
# Circuit : c5315
# Loops	  : 1
# length  : 3
script c5315res_obf13.txt time ./decrypto.sh c5315.bench 1 3
# "CPU time"の行を取り出す
cat c5315res_obf13.txt | grep CPU > c5315time_obf13.txt
exit


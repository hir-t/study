# Circuit : c5315
# Loops	  : 1
# length  : 6
script c5315res_obf16.txt time ./decrypto.sh c5315.bench 1 6
# "CPU time"の行を取り出す
cat c5315res_obf16.txt | grep CPU > c5315time_obf16.txt
exit


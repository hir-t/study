# Circuit : c5315
# Loops	  : 6
# length  : 6
script c5315res_obf66.txt time ./decrypto.sh c5315.bench 6 6
# "CPU time"の行を取り出す
cat c5315res_obf66.txt | grep CPU > c5315time_obf66.txt
exit


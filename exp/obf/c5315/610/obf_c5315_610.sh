# Circuit : c5315
# Loops	  : 6
# length  : 10
script c5315res_obf610.txt time ./decrypto.sh c5315.bench 6 10
# "CPU time"の行を取り出す
cat c5315res_obf610.txt | grep CPU > c5315time_obf610.txt
exit


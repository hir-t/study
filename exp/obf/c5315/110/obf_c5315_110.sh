# Circuit : c5315
# Loops	  : 1
# length  : 10
script c5315res_obf110.txt time ./decrypto.sh c5315.bench 1 10
# "CPU time"の行を取り出す
cat c5315res_obf110.txt | grep CPU > c5315time_obf110.txt
exit


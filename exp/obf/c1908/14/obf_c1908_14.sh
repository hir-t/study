# Circuit : c1908
# Loops	  : 1
# length  : 4
script c1908res_obf14.txt time ./decrypto.sh c1908.bench 1 4
# "CPU time"の行を取り出す
cat c1908res_obf14.txt | grep CPU > c1908time_obf14.txt
exit


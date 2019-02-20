# Circuit : c1908
# Loops	  : 6
# length  : 4
script c1908res_obf64.txt time ./decrypto.sh c1908.bench 6 4
# "CPU time"の行を取り出す
cat c1908res_obf64.txt | grep CPU > c1908time_obf64.txt
exit


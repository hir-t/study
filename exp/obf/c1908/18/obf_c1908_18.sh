# Circuit : c1908
# Loops	  : 1
# length  : 8
script c1908res_obf18.txt time ./decrypto.sh c1908.bench 1 8
# "CPU time"の行を取り出す
cat c1908res_obf18.txt | grep CPU > c1908time_obf18.txt
exit


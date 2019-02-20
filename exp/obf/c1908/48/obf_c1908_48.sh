# Circuit : c1908
# Loops	  : 4
# length  : 8
script c1908res_obf18.txt time ./decrypto.sh c1908.bench 4 8
# "CPU time"の行を取り出す
cat c1908res_obf48.txt | grep CPU > c1908time_obf48.txt
exit


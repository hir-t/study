# Circuit : c1908
# Loops	  : 3
# length  : 6
script c1908res_obf36.txt time ./decrypto.sh c1908.bench 3 6
# "CPU time"の行を取り出す
cat c1908res_obf36.txt | grep CPU > c1908time_obf36.txt
exit


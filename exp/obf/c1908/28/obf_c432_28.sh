# Circuit : c1908
# Loops	  : 2
# length  : 8
script c1908res_obf28.txt time ./decrypto.sh c1908.bench 2 8
# "CPU time"の行を取り出す
cat c1908res_obf28.txt | grep CPU > c1908time_obf28.txt
exit


# Circuit : c1908
# Loops	  : 3
# length  : 8
script c1908res_obf38.txt time ./decrypto.sh c1908.bench 3 8
# "CPU time"の行を取り出す
cat c1908res_obf38.txt | grep CPU > c1908time_obf38.txt
exit


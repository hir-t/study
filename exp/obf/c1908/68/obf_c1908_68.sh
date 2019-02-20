# Circuit : c1908
# Loops	  : 6
# length  : 8
script c1908res_obf68.txt time ./decrypto.sh c1908.bench 6 8
# "CPU time"の行を取り出す
cat c1908res_obf68.txt | grep CPU > c1908time_obf68.txt
exit


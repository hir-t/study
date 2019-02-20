# Circuit : c1908
# Loops	  : 8
# length  : 8
script c1908res_obf88.txt time ./decrypto.sh c1908.bench 8 8
# "CPU time"の行を取り出す
cat c1908res_obf88.txt | grep CPU > c1908time_obf88.txt
exit


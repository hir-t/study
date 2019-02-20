# Circuit : c1908
# Loops	  : 3
# length  : 10
script c1908res_obf310.txt time ./decrypto.sh c1908.bench 3 10
# "CPU time"の行を取り出す
cat c1908res_obf310.txt | grep CPU > c1908time_obf310.txt
exit


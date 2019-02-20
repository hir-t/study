# Circuit : c1908
# Loops	  : 1
# length  : 10
script c1908res_obf110.txt time ./decrypto.sh c1908.bench 1 10
# "CPU time"の行を取り出す
cat c1908res_obf110.txt | grep CPU > c1908time_obf110.txt
exit


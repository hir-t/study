# Circuit : c1908
# Loops	  : 8
# length  : 10
script c1908res_obf810.txt time ./decrypto.sh c1908.bench 8 10
# "CPU time"の行を取り出す
cat c1908res_obf810.txt | grep CPU > c1908time_obf810.txt
exit


# Circuit : c1908
# Loops	  : 4
# length  : 10
script c1908res_obf410.txt time ./decrypto.sh c1908.bench 4 10
# "CPU time"の行を取り出す
cat c1908res_obf410.txt | grep CPU > c1908time_obf410.txt
exit


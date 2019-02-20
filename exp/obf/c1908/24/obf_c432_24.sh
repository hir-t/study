# Circuit : c1908
# Loops	  : 2
# length  : 4
script c1908res_obf24.txt time ./decrypto.sh c1908.bench 2 4
# "CPU time"の行を取り出す
cat c1908res_obf24.txt | grep CPU > c1908time_obf24.txt
exit


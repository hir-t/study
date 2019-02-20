# Circuit : c1908
# Loops	  : 4
# length  : 6
script c1908res_obf46.txt time ./decrypto.sh c1908.bench 4 6
# "CPU time"の行を取り出す
cat c1908res_obf46.txt | grep CPU > c1908time_obf46.txt
exit


# Circuit : c1908
# Loops	  : 3
# length  : 4
script c1908res_obf34.txt time ./decrypto.sh c1908.bench 3 4
# "CPU time"の行を取り出す
cat c1908res_obf34.txt | grep CPU > c1908time_obf34.txt
exit


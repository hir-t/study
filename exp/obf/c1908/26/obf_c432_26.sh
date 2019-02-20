# Circuit : c1908
# Loops	  : 2
# length  : 6
script c1908res_obf26.txt time ./decrypto.sh c1908.bench 2 6
# "CPU time"の行を取り出す
cat c1908res_obf26.txt | grep CPU > c1908time_obf26.txt
exit


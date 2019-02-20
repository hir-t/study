# Circuit : c1908
# Loops	  : 2
# length  : 10
script c1908res_obf210.txt time ./decrypto.sh c1908.bench 2 10
# "CPU time"の行を取り出す
cat c1908res_obf210.txt | grep CPU > c1908time_obf210.txt
exit


# Circuit : c1908
# Loops	  : 8
# length  : 4
script c1908res_obf84.txt time ./decrypto.sh c1908.bench 8 4
# "CPU time"の行を取り出す
cat c1908res_obf84.txt | grep CPU > c1908time_obf84.txt
exit


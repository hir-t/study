# Circuit : c1908
# Loops	  : 6
# length  : 6
script c1908res_obf66.txt time ./decrypto.sh c1908.bench 6 6
# "CPU time"の行を取り出す
cat c1908res_obf66.txt | grep CPU > c1908time_obf66.txt
exit


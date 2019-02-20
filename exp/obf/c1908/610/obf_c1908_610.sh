# Circuit : c1908
# Loops	  : 6
# length  : 10
script c1908res_obf610.txt time ./decrypto.sh c1908.bench 6 10
# "CPU time"の行を取り出す
cat c1908res_obf610.txt | grep CPU > c1908time_obf610.txt
exit


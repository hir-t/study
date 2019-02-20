# decryoto
# Circuit : c1908
# Loops	  : 8
# length  : 3
script c1908res_obf83.txt time ./decrypto.sh c1908.bench 8 3
# "CPU time"の行を取り出す
cat c1908res_obf83.txt | grep CPU > c1908time_obf83.txt
exit


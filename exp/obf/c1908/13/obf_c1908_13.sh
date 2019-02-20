# decryoto
# Circuit : c1908
# Loops	  : 1
# length  : 3
script c1908res_obf13.txt time ./decrypto.sh c1908.bench 1 3
# "CPU time"の行を取り出す
cat c1908res_obf13.txt | grep CPU > c1908time_obf13.txt
exit


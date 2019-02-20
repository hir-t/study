# decryoto
# Circuit : c1908
# Loops	  : 4
# length  : 3
script c1908res_obf43.txt time ./decrypto.sh c1908.bench 4 3
# "CPU time"の行を取り出す
cat c1908res_obf43.txt | grep CPU > c1908time_obf43.txt
exit


# decryoto
# Circuit : c1908
# Loops	  : 3
# length  : 3
script c1908res_obf33.txt time ./decrypto.sh c1908.bench 3 3
# "CPU time"の行を取り出す
cat c1908res_obf33.txt | grep CPU > c1908time_obf33.txt
exit


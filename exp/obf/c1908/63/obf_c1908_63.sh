# decryoto
# Circuit : c1908
# Loops	  : 6
# length  : 3
script c1908res_obf63.txt time ./decrypto.sh c1908.bench 6 3
# "CPU time"の行を取り出す
cat c1908res_obf63.txt | grep CPU > c1908time_obf63.txt
exit


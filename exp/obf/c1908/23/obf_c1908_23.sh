# decryoto
# Circuit : c1908
# Loops	  : 2
# length  : 3
script c1908res_obf23.txt time ./decrypto.sh c1908.bench 2 3
# "CPU time"の行を取り出す
cat c1908res_obf23.txt | grep CPU > c1908time_obf23.txt
exit

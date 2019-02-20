# decryoto
# Circuit : c1908
# Loops	  : 4
# length  : 4
script c1908res_obf44.txt time ./decrypto.sh c1908.bench 4 4
# "CPU time"の行を取り出す
cat c1908res_obf44.txt | grep CPU > c1908time_obf44.txt
exit


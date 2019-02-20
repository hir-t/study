# Circuit : c1908
# Loops	  : 8
# length  : 6
script c1908res_obf86.txt time ./decrypto.sh c1908.bench 8 6
# "CPU time"の行を取り出す
cat c1908res_obf86.txt | grep CPU > c1908time_obf86.txt
exit


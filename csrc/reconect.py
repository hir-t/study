f = open('c17.bench', 'r')
for line in f:
	for a in line:
		a = line.split()
    	print a
f.close()
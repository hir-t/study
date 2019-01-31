#!/usr/bin/env python
# -*- coding: utf-8 -*-
f1 = open('eval.cnf', 'r')
f2 = open('c1.muxOutinfo', 'r')
f3 = open('test.txt', 'w')
print "ooooooo"
# for line1 in f1:
# 	for a  in line1.split():
# 		#print "a = " + a
# 		for line2 in f2:
# 			for b in line2.split():
# 				print "a = " + a
# 				print "b = " + b
data = []
data2 = []		#cnfファイルをスペースごと読む
e_line = []		#cnfファイルを１行ずつ読み込む
for line1 in f2:
	for a  in line1.split():
		data = data + [a]
		#print data


for line2 in f1:
	e_line = e_line + [line2]
	print e_line
	for a  in line2.split():
		data2 = data2 + [a]
		#print data2

num = sum(1 for e in data)
#print num

for e in data:
	print e

print "--------"
for e in data2:
	print e

f1.close()
f2.close()
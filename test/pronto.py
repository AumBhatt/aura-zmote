#!/usr/bin/env python

'''python gc7.py "192.168.1.110,sendir,1:3,1,1,0000 006C 0000 0032 015B 00AD 0016 0041 0016 0016 0016 0041 0016 0041 0016 0016 0016 0016 0016 0041 0016 0016 0016 0016 0016 0041 0016 0016 0016 0016 0016 0041 0016 0041 0016 0016 0016 0041 0016 0041 0016 0016 0016 0016 0016 0041 0016 0041 0016 0041 0016 0041 0016 0041 0016 0016 0016 0041 0016 0041 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0041 0016 0041 0016 0016 0016 0016 0016 0041 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0041 0016 0016 0016 0041 0016 0041 0016 01E7'''
#sendir,1:3,1,38380,1,1,347,173,22,65,22,22,22,65,22,65,22,22,22,22,22,65,22,22,22,22,22,65,22,22,22,22,22,65,22,65,22,22,22,65,22,65,22,22,22,22,22,65,22,65,22,65,22,65,22,65,22,22,22,65,22,65,22,22,22,22,22,22,22,22,22,22,22,65,22,65,22,65,22,22,22,22,22,65,22,22,22,22,22,22,22,22,22,22,22,65,22,65,22,22,22,65,22,65,22,487
#completeir,1:3,1
'''
--------format----------
python gc7.py "<ip_address>,<command>,<channel>,<repeat>,<offset>,<code>"
------------------------
'''

'''
------- CHANNEL FORMAT --------
channel_format = "x:y"
where 'x' = 1(for all except Global Cache models)
and 'y' = <channel_number> [0, n]
'''

import socket
from time import sleep
import sys
PORT_ONE=4998
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

v=sys.argv[1]
v=v.split(",")
HOST=v[0]
if(v[1] == "sendir"):
	s=v[1]
	s=s+','+v[2]+','+'1'+','
	w=v[5].replace(" ","")
	if(w[0:4] == "0000"):
		frequency=int(1000000/(int(w[4:8],16)*0.241246))
		s=s+str(frequency)+','
	s=s+str(v[3])+','+str(v[4])
	for i in range (16,len(w),4):
	    s += ','+str(int(w[i:i+4],16))
	s=s+"\r"
	print(s)	    

	try:
		sock.connect((HOST, PORT_ONE))
		sock.settimeout(2)
		sock.sendall(s)
		msg = sock.recv(4096)
		print msg
		sock.close()
		del sock
	except Exception as e:
			print(e)

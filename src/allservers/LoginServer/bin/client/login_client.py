import socket, sys, struct

if __name__ == '__main__':
	argv = sys.argv
	if(len(argv) != 5) or (len(argv) == 3 and argv[1] == '/?'):
		print '>>>Useage:', argv[0], '<address> <port>'
		sys.exit(0)
	
	HOST = argv[1]
	PORT = int(argv[2])
	ProxyHost = 0
	ProxyPort = 0
	UserAccount = argv[3]
	UserPassword = argv[4]

	print '>>>HOST:', HOST
	print '>>>PORT:', PORT

	try:
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	except socket.error:
		print '>>>Fail to open socket!'
		sys.exit(1)

	print '>>>Socket opened...'

	try:
		sock.connect((HOST, PORT))
	except socket.error:
		print '>>>Fail to connect!'
		sock.close()
		print '>>>Socket closed...'
		sys.exit(1)
	
	print '>>>Socket connected...'

	try:
		c2s_login = struct.pack('=LH16s16s', 3, 2000, UserAccount, UserPassword)
		sock.send(c2s_login)
	except socket.error:
		print '>>>Fail to send login protocal'
		sys.exit(2)

	print 'Waiting to recv protocal'
		
	try:
		msg = sock.recv(1024)
		print len(msg)
		#print msg
		print struct.calcsize('=LHL')
		s2c_login = struct.unpack('=LHL', msg)
		print s2c_login

		if s2c_login[1] == 2001:
			if s2c_login[2] == 0:
				print '>>>User login failed.'
				sock.close()
				sys.exit(1)
		
		print ">>>User login."
		
	except socket.error:
		print '>>>Fail to receive message!'

	try:
		msg = sock.recv(1024)
		print len(msg)
		# print struct.calcsize('=LHLHLL')
		s2c_proxy_info = struct.unpack('=LHLHLL', msg)
		print s2c_proxy_info
		print s2c_proxy_info[3]

		ProxyHost = s2c_proxy_info[3];
		ProxyPort = s2c_proxy_info[4];

	except socket.error:
		print '>>>Fail to receive message!'
		
		
	
	# tmp = raw_input()

	sock.close()

	pass

	# sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	# sock.connect(('192.168.0.106', ProxyPort))

	#tmp = raw_input();

	#sock.close()
		

		

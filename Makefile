all:
	gcc -o bin/client src/client/client.c
	gcc -o bin/generic_server src/servers/generic_server.c

bs:
	gcc -o bin/generic_server src/servers/generic_server.c
bc:
	gcc -o bin/client src/client/client.c
runs:
	bin/generic_server
runc:
	bin/client 0.0.0.0 8080 /test/img.jpg 1 10
sobel:

compile:
	gcc src/client/ClienteHolla.c -o bin/ClienteHolla -pthread
	gcc src/servers/Hilos.c -o bin/Hilos -pthread
	gcc src/servers/Folk.c -o bin/Folk -pthread
	gcc src/client/Clientimg.c -o bin/Clientimg -pthread
	gcc src/servers/Secuencial.c -o bin/Secuencial -pthread
h:
	bin/Hilos
f: 
	bin/Folk
ss:
	bin/Secuencial
e:
	bin/ClienteHolla 5 1
ee:
	bin/Clientimg 2 2
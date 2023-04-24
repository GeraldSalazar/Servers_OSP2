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
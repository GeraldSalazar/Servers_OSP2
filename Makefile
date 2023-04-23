all:
	gcc -o bin/client src/client/client.c -lrt -lpthread
	gcc -o bin/generic_server Native_Sobel/src/file_operations.c Native_Sobel/src/image_operations.c Native_Sobel/src/Sobel.c -lm src/servers/generic_server.c -lrt -lpthread

bs:
	gcc -o bin/generic_server src/servers/generic_server.c
bc:
	gcc -o bin/client src/client/client.c
runs:
	bin/generic_server
runc:
	bin/client 0.0.0.0 8080 /test/img.jpg 1 10
sobel:
	gcc Native_Sobel/src/file_operations.c Native_Sobel/src/image_operations.c Native_Sobel/src/main.c -lm -o bin/Native_Sobel
run_sobel:
	bin/Native_Sobel assets/sample_1_640x427.jpg

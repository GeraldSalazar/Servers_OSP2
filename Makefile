all:
	gcc -o bin/client src/client/client.c
	gcc -o bin/secuencial Native_Sobel/src/file_operations.c Native_Sobel/src/image_operations.c Native_Sobel/src/Sobel.c -lm src/servers/secuencial.c
	gcc -o bin/hilos Native_Sobel/src/file_operations.c Native_Sobel/src/image_operations.c Native_Sobel/src/Sobel.c -lm src/servers/hilos.c
	gcc -o bin/folk Native_Sobel/src/file_operations.c Native_Sobel/src/image_operations.c Native_Sobel/src/Sobel.c -lm src/servers/folk.c
	gcc -o bin/secuencial src/servers/secuencial.c

bc:
	gcc -o bin/client src/client/client.c

runs:
	bin/secuencial
runh:
	bin/hilos
runf:
	bin/folk
runcs:
	bin/client 0.0.0.0 8000 assets/sample_1_640x427.jpg 3 3

runch:
	bin/client 127.0.0.1 9000 assets/sample_2_1280x853.jpg 2 2

runcf:
	bin/client 127.0.1.2 5000 assets/sample_2_1280x853.jpg 2 2

clean:
	rm TestImg/* -rf
	rm filtered/* -rf
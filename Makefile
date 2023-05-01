all:
	gcc -o bin/client src/client/client.c
	gcc -o bin/secuencial Native_Sobel/src/file_operations.c Native_Sobel/src/image_operations.c Native_Sobel/src/Sobel.c -lm src/servers/secuencial.c
	gcc -o bin/hilos Native_Sobel/src/file_operations.c Native_Sobel/src/image_operations.c Native_Sobel/src/Sobel.c -lm src/servers/hilos.c
	gcc -o bin/folk Native_Sobel/src/file_operations.c Native_Sobel/src/image_operations.c Native_Sobel/src/Sobel.c -lm src/servers/folk.c
 
bc:
	gcc -o bin/client src/client/client.c

runs:
	bin/secuencial
runh:
	bin/hilos
runf:
	bin/folk
runcs:
	bin/client 0.0.0.0 8000 assets/exam.jpg 2 2
runcs2:
	bin/client 0.0.0.0 8000 assets/sample_2_1280x853.jpg 1 1
runcs3:
	bin/client 0.0.0.0 8000 assets/Sekiro.jpg 1 1

runch:
	bin/client 127.0.0.1 9000 assets/sample_2_1280x853.jpg 2 2

runcf:
	bin/client 127.0.0.2 5000 assets/sample_2_1280x853.jpg 2 2

clean:
	rm TestImg/* -rf
	rm filtered/* -rf
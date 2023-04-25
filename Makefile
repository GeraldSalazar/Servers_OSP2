all:
	gcc -o bin/client src/client/client.c
	gcc -o bin/secuencial src/servers/secuencial.c
	gcc -o bin/hilos src/servers/hilos.c
	gcc -o bin/folk src/servers/folk.c
bs:
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
	bin/client 0.0.0.0 8080 assets/sample_2_1280x853.jpg 1 3

runch:
	bin/client 127.0.0.1 9000 assets/sample_2_1280x853.jpg 1 2

runcf:
	bin/client 127.0.0.2 5000 assets/sample_2_1280x853.jpg 1 2

clean:
	rm TestImg/* -rf
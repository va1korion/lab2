all:
	gcc -o lab2server server.c -lpthread
	gcc -o lab2client client.c

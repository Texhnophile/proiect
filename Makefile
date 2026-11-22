CC = gcc
CFLAGS = -Wall -g -Wextra
proiect: main.c bmp_io_rgb.h 
		$(CC) $(CFLAGS) main.c -o proiect
clean:
		rm -rf proiect

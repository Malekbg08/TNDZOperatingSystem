CC=gcc
CFLAGS=-std=c99 -Wextra -Wall  -pedantic 
LDFLAGS=

all:clean exe d0

exe:	couche1.c	couche2.c	couche3.c	couche4.c	couche5.c	sha256.c	sha256_utils.c
	$(CC)	-o	exe	couche1.c	couche2.c	couche3.c	couche4.c	couche5.c	sha256.c	sha256_utils.c

d0:FORCE
	./cmd_format	.	500000

FORCE:

cmd_format:	cmd_format.c
	$(CC)	cmd_format.c	-o	cmd_format
clean:
	@rm -rf *.o
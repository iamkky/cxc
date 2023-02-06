include ../make.cx.inc

CC = gcc
CFLAGS+=-Wall
LDFLAGS+=-labd -lhelium

all: form

form: form.o
	$(CC) $(LDFLAGS) -o form form.o -lhelium -labd

clean:
	rm -f *.o teste form 




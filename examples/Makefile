include ../make.cx.inc

CC = gcc
CFLAGS+=-Wall
LDFLAGS+=-labd -lhelium

all: form form2

form: form.o
	$(CC) $(LDFLAGS) -o form form.o -lhelium -labd

form2: form2.o
	$(CC) $(LDFLAGS) -o form2 form2.o -lhelium -labd

clean:
	rm -f *.o teste form 




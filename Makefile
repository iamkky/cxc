
NRLEX=nrlex
RDPPGEN=rdppgen

CFLAGS+=-I. -gstabs

all: libcx.a cxc

cxc: cxc.o libcx.a
	${CC} ${CFLAGS} -o cxc cxc.o libcx.a

libcx.a: cx_lex.nrlex cx_parser.rdpp cx_component.o strf.o
	${RDPPGEN} cx_parser.rdpp
	${NRLEX} -H cx_lex.h < cx_lex.nrlex > cx_lex.c
	${CC} ${CFLAGS} -c cx_parser.c
	${CC} ${CFLAGS} -c cx_lex.c
	${AR} rcs libcx.a cx_lex.o cx_parser.o cx_component.o strf.o

clean:
	rm -f *.o cx_lex.c cx_lex.h cx_parser.c cx_parser.h cx_parser.tokens.h cxc libcx.a 


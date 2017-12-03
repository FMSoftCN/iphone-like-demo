INCLUDES=-I..

OBJS=pcc_help.o y.tab.o lex.yy.o

all:$(OBJS) y.tab.h pcc.h
	$(CC) -o pcc $(OBJS)

y.tab.c:pcc.y
	yacc -d pcc.y

y.tab.h:pcc.y
	yacc -d pcc.y

lex.yy.c:pcc.l y.tab.h
	lex pcc.l

clean:
	rm -f *.o pcc lex.yy.c y.tab.[ch]

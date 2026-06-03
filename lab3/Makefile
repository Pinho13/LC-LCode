# name of the program (Minix service)
PROG=lab3

# source code files to be compiled
# timer.c is compiled directly so lab3 does not depend on lab2's libtimer.a
SRCS = lab3.c kbc.c utils.c ../lab2/timer.c

# additional compilation flags
CFLAGS += -pedantic
CFLAGS += -DLAB3
CFLAGS += -I../lab2

# list of library dependencies (LCF library only)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>

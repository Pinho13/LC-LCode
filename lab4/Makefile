# name of the program (Minix service)
PROG=lab4

# source files
SRCS = lab4.c mouse.c utils.c ../lab2/timer.c

# compilation flags
CFLAGS += -pedantic
CFLAGS += -DLAB4
CFLAGS += -I../lab2

# LCF library
DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>

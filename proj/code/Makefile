# name of the program (Minix service)
PROG=proj

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -D __LCOM_OPTIMIZED__

# list of library dependencies (LCF)
DPADD += ${LIBLCF}
LDADD += -llcf
LDLIBS += -lm

# define headers dependencies
HDRS = $(find . -name *.h)

# compilation rules for each source file
proj.o: proj.c ${HDRS}
	${CC} ${CFLAGS} -c $< -o $@

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
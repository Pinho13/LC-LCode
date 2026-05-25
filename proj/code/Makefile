# name of the program (Minix service)
PROG=proj

# Framework path
FRAMEWORK = ../../framework/liblcomfw.a
FRAMEWORK_INCLUDES = ../../framework/includes

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -I${FRAMEWORK_INCLUDES}

# list of library dependencies (LCF)
DPADD += ${FRAMEWORK} ${LIBLCF}
LDADD += ${FRAMEWORK} -llcf
LDLIBS += -lm

# define headers dependencies
HDRS = $(find . -name *.h)

# compilation rules for each source file
proj.o: proj.c ${HDRS}
	${CC} ${CFLAGS} -c $< -o $@

controller/ih/ih.o: controller/ih/ih.c controller/ih/ih.h
	${CC} ${CFLAGS} -c $< -o $@

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
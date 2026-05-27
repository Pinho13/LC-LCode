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
HDRS = $(find . -name *.c)

SRCS = proj.c ih.c
OBJS = ${SRCS:.c=.o}
.PATH: controller/ih

.PHONY: run

run: all
	for obj in $(OBJS); do \
		mkdir -p build; \
		mv $$obj build/$$obj; \
	done

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
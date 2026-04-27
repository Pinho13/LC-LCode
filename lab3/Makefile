# name of the program (Minix service)
PROG=lab3
# source code files to be compiled
SRCS = lab3.c kbc.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -DLAB3 -I../lab2


DPADD += ${LIBLCF} ${LIB_NAME}              # Let make track the versions of the new target
LDADD += -llcf -L. -L../lab2                        
LDADD += -lkbc -ltimer                           #   and link the library named libtimers.a

LIB_NAME = libkbc.a                       # The name filename for your library
LIB_SRCS = utils.c keyboard.c                 # The source files to include
LIB_OBJS = ${LIB_SRCS:.c=.o}                # The rule used to convert your .c source files in .o object files
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}       # Add the library file and object files to the files be cleaned

all: lib                                    # Instruct make run the lib compilation target before building the lab3 program
lib: ${LIB_OBJS}                            
	ar rcs ${LIB_NAME} ${LIB_OBJS}          
# ^ This MUST be a Tab character, not spaces


# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>



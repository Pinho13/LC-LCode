# name of the program (Minix service)
PROG = lab5

# source code files to be compiled
SRCS = lab5.c video.c kbc.c utils.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic
CFLAGS += -DLAB5

DPADD += ${LIBLCF} ${LIB_NAME}             # Let make track the versions of the new target
LDADD += -llcf -L.                         # Tell the compiler to look in the current directory for libraries
LDADD += -lvideo                           #   and link the library named libvideo.a

LIB_NAME = libvideo.a                      # The name filename for your library
LIB_SRCS = video.c kbc.c utils.c          # The source files to include
LIB_OBJS = ${LIB_SRCS:.c=.o}              # The rule used to convert your .c source files in .o object files
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}     # Add the library file and object files to the files to be cleaned

all: lib                                   # Instruct make run the lib compilation target before building the lab5 program
lib: ${LIB_OBJS}                           # Define the lib compilation target, with its dependency source files
	ar rcs ${LIB_NAME} ${LIB_OBJS}

# ^ This MUST be a Tab character, not spaces

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>

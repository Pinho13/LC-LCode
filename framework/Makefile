CC ?= clang
AR ?= ar

LIB_NAME := liblcomfw.a

INCLUDE_DIR := includes

LIB_SRCS := src/common/utils.c \
            src/drivers/rtc.c \
            src/drivers/timer.c
LIB_OBJS := $(LIB_SRCS:.c=.o)

PROG := ex_rtc
SRCS := ex_rtc.c
.PATH: examples

CFLAGS += -std=c11 -Wall -Wextra -Werror \
           -pedantic -I$(INCLUDE_DIR)
DPADD += ${LIBLCF}
LDADD += -L. -llcomfw -llcf

.PHONY: all examples

all: $(LIB_NAME)

# Build library
$(LIB_NAME): $(LIB_OBJS)
	$(AR) rcs $@ $(LIB_OBJS)

# Compile object files (explicit rules for Minix make compatibility)
src/common/utils.o: src/common/utils.c
	$(CC) $(CFLAGS) -c $< -o $@

src/drivers/rtc.o: src/drivers/rtc.c
	$(CC) $(CFLAGS) -c $< -o $@

src/drivers/timer.o: src/drivers/timer.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build all examples
examples: $(LIB_NAME) $(PROG)

CLEANFILES += $(LIB_OBJS) $(LIB_NAME)

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>

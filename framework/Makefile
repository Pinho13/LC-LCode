CC ?= gcc
AR ?= ar

LIB_NAME := liblcomfw.a
INCLUDE_DIR := includes
SRC_DIR := src

SRCS := $(wildcard $(SRC_DIR)/common/*.c) \
        $(wildcard $(SRC_DIR)/drivers/*.c)
OBJS := $(SRCS:.c=.o)

CFLAGS += -std=c11 -Wall -Wextra -Werror -pedantic -I$(INCLUDE_DIR)

.PHONY: all clean

all: $(LIB_NAME)

$(LIB_NAME): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIB_NAME)

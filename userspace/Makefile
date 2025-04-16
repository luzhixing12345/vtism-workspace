# 编译器和参数
CC = gcc
# CFLAGS = -O3 -Wall
LDFLAGS = -lnuma -lpthread

# 所有的 .c 文件
SRCS := $(wildcard *.c)

# 对应的可执行文件（去掉.c后缀）
TARGETS := $(SRCS:.c=)

# 默认目标：编译所有
all: $(TARGETS)

CFLAGS=-O0

# 每个目标的编译规则
%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# 清理
clean:
	rm -f $(TARGETS)

.PHONY: all clean

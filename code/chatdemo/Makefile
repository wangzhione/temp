# Linux / macOS / MinGW 通用 Makefile, 用于快速构建聊天室服务端与客户端

CC := gcc
CSTD_FLAG := $(shell printf '' | $(CC) -std=c23 -x c - -c -o /dev/null >/dev/null 2>&1 && echo -std=c23 || echo -std=c2x)
CFLAGS ?= $(CSTD_FLAG) -Wall -Wextra -pedantic
CPPFLAGS ?= -Iinc

UNAME_S := $(shell uname -s 2>/dev/null)

SRC_COMMON := \
	src/platform.c \
	src/socketx.c \
	src/threadx.c \
	src/utf8.c \
	src/chat_protocol.c \
	src/chat_server.c \
	src/chat_client.c

OBJ_DIR := build_make/obj
DEPFLAGS := -MMD -MP

ifeq ($(OS),Windows_NT)
EXE := .exe
LDLIBS += -lws2_32 -lshell32
else
EXE :=
LDLIBS += -pthread
endif

SERVER_SRCS := $(SRC_COMMON) src/server_main.c
CLIENT_SRCS := $(SRC_COMMON) src/client_main.c

SERVER_OBJS := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SERVER_SRCS))
CLIENT_OBJS := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(CLIENT_SRCS))
DEPS := $(SERVER_OBJS:.o=.d) $(CLIENT_OBJS:.o=.d)

.PHONY: all clean

all: chat_server$(EXE) chat_client$(EXE)

chat_server$(EXE): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

chat_client$(EXE): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	rm -rf build_make chat_server chat_client chat_server.exe chat_client.exe

-include $(DEPS)

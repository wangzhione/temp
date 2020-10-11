#
# make
#
# Debug   : make
# Clean   : make clean
# Release : make D=-DNDEBUG
#

#
# SRCC    : 所有 *.c 文件
# OBJO    : 所有 *.o 文件
#
SRCC      = $(wildcard *.c)
OBJO      = $(foreach v, $(SRCC),$(notdir $(basename $(v))).o)

#
# 全局编译的设置
#
CC        ?= cc
CFLAGS    := -g -O2 -Wall -Wextra -Wimplicit-fallthrough=0 -Wno-sign-compare -D_GNU_SOURCE
LIB       := -lm

HEAD    := $(CC) $(CFLAGS)
ifneq ( , $(D))
	HEAD+=$(D)
endif

TAIL      = $(OBJO) $(LIB)

RUNO      = $(HEAD) -c -o $@ $<
RUN       = $(HEAD) -o $@ $(TAIL)

#
# 具体的产品生产
#
all : main

#
# 主运行程序 main
#
main : $(OBJO)
	$(RUN)

#
# 循环产生 -> 所有 - 链接文件 *.o
#
define CALL_RUNO
$(notdir $(basename $(1))).o : $(1)
	$$(RUNO)
endef

$(foreach v, $(SRCC), $(eval $(call CALL_RUNO, $(v))))

.PHONY: clean

# 清除操作
clean :
	-rm -rf *.o
	-rm -rf main

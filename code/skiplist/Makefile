#
#
#

#
# 全局编译的设置
#
CC        ?= cc
#
# -Werror                           : 使所有警告变为错误
# -Wall -Wextra                     : 启用一些很有用的警告选项
# -Wimplicit-fallthrough=0          : 忽略 switch case missing break warning
# -Wno-missing-field-initializers   : 忽略 initializer for field warning
#
CFLAGS    := -g -O2 -Werror -Wall -Wextra -Wimplicit-fallthrough=0 -Wno-missing-field-initializers -D_GNU_SOURCE
LIB       := -lm -lpthread

HEAD    := $(CC) $(CFLAGS)
ifneq ( , $(D))
	HEAD+=$(D)
endif

RUNO      = $(HEAD) $(IINC) -c -o $@ $^
RUN       = $(HEAD) $(IINC) -o $@ $^ $(LIB)

#
# 具体的产品生产
#
.PHONY : all clean

all : main.out

main.out : main.c
	cc -g -O2 -Wall -Wextra -D_GNU_SOURCE -o $@ $^
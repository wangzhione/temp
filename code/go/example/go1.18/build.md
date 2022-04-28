# go build

## go build 编译时附加参数

```
-v      : 编译时显示包名
-p n    : 开启并发编译, 默认下该值为 CPU 逻辑核数
-a      : 强制重新构建
-n      : 打印编译时会用到的所有命令, 但不真正执行
-x      : 打印编译时会用到的所有命令
-race   : 开启竞态检测
-work   : 打印编译时生成的临时工作目录路径, 并在编译结束时保留他. 默认情况编译结束会删除.
```

```
-asmflags
: 控制 Go 语言编译器编译汇编语言文件时的行为

-buildmode
: 指定编译模式, 支持六种编译模式

-compiler
: 指定编译器, 其值可为 gc 或 gccgo, gc 编译器即为 Go 语言自带编译器, gccgo 为 GCC 提供的编译器

-gccgoflags 'gccgoflags list'
: 指定需要传递给 gccgo 编译器或连接器的标记列表

-gcflags 'gcflags list'
: 指定需要传递给 go tool compile 命令的标记的列表

-installsuffix suffix
: 使当前的输出目录与默认的编译输出目录分离, 此标记的值会作为结果文件的父目录名称的后缀

-ldflags 'ldflags list'
: 指定需要传递给 go toll link 命令的标记的列表

-tags 'tags list'
: 指定在实际编译期间需要受理的编译标签(也课被成为编译约束)的列表
```

例如 **-ldflags "-w -s"** 选项目的是获取最小的二进制文件

```
$ go tool link --help

-s      : 关闭符号表生成和调试信息
-w      : 关闭 DWAPP 调试信息, 无法二进制文件上使用 gdb 的特定功能和设置断点获取堆栈跟踪信息
```

**-gcglags "-N -l"** 选项目的是在编译过程中禁止内联优化, 加快编译速度减少开销.

```
$ go tool compile --help

-N      : 禁止编译优化
-l      : 禁止内联, 一定程度减少可执行程序大小
```

**go tool compile -N -l -S once.go 生成汇编代码** 其中 **go tool objdump once.o** 查看
或 "go tool objdump -s Do once.o反汇编特定的函数"

也可以 **go build -gcflags -S once.go** 可以得到汇编代码.

go tool compile 和 go build -gcflags -S 生成的是过程中的汇编，和最终的机器码的汇编可以通过go tool objdump生成。

GOSSAFUNC=main GOOS=linux GOARCH=amd64 go tool compile main.go

package main

/*
 #cgo LDFLAGS:-L/usr/local/lib

 #include <stdio.h>
 #include <stdlib.h>

 // CGO 会保留 C 代码块中的宏定义
 #define REPEAT_LIMIT 3

 typedef struct {
	 int repeat_time;
	 char * str;
 } blob;

 int say_hello(blob * ptr) {
	 while (ptr->repeat_time < REPEAT_LIMIT) {
		 puts(ptr->str);
		 ptr->repeat_time++;
	 }
	 return 0;
 }
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// build:
// go run 01_cgo_go2c.go
func main() {
	// 在 Go 程序中创建 C 对象, 存储在 Go 的内存空间
	cblob := C.blob{repeat_time: 0}
	// C.CString 会在 C 的内存空间申请一个 C 语言字符串对象, 在将 Go 字符串拷贝到 C 字符串
	cblob.str = C.CString("Hello, World\n")

	res := C.say_hello(&cblob)

	fmt.Println("res:", res)
	fmt.Println("repeat_time:", cblob.repeat_time)

	// C.CString 申请的 C 空间内存不足自动释放, 需要显示调用 C 中的 free 释放
	C.free(unsafe.Pointer(cblob.str))
}

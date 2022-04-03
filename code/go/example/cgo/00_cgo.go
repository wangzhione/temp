package main

////import "C" 更像是一个关键词, CGO 工具在预处理会删除这一行
//#include <stdio.h> // 序文件中可以链接标准 C 程序库
import "C"

// build :
// -x 选项可以查看 go 程序编译过程中执行所有指令
// go build -x 00_cgo.go
func main() {
	C.puts(C.CString("Hello, CGO"))
}

// 能够调用 C 语言代码的 Go 语言源文件，也就是说所有启用了 CGO 特性的 Go 代码，都会首先经过 cgo 的"预处理"
// go tool cgo 00_cgo.go
//

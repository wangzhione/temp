package main

import (
	"fmt"
	"unsafe"
)

// T1 类型的尺寸在64位架构上为24个字节（1+7+8+2+6），
// 在32位架构上为16个字节（1+3+8+2+2）。
type T1 struct {
	a int8

	// 在64位架构上，为了让字段b的地址为8字节对齐，
	// 需在这里填充7个字节。在32位架构上，为了让
	// 字段b的地址为4字节对齐，需在这里填充3个字节。
	b int64
	c int16

	// 为了让类型T1的尺寸为T1的对齐保证的倍数，
	// 在64位架构上需在这里填充6个字节，在32架构
	// 上需在这里填充2个字节。
}

func main() {
	fmt.Println(unsafe.Sizeof(T1{}))
}

// GOOS=windows GOARCH=386 go build sizeof.go
// GOOS=windows GOARCH=amd64 go build sizeof.go

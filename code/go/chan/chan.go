package main

import (
	"fmt"
	"net"
)

import "C"

var temp net.IP

func main() {
	fmt.Println("start ... .. .")
	ch := make(chan int)
	<-ch
	fmt.Println("Hello, 世界", temp)
}

/*
 go version go1.18.2windows/amd64

 go build chan.go	success

 go run chan.go		error
 start ... .. .
 fatal error: all goroutines are asleep - deadlock!

 goroutine 1 [chan receive]:
 main.main()
 		D:/code/github/temp/code/go/chan/chan.go:8 +0x6c
 exit status 2

 我的环境没有 CGO 所以还是会开启 检查
*/

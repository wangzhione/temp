package main

import (
	"log"
	"os"
	"runtime/trace"
)

// build:
// go run 1502_goroutine.go
//
func main() {
	// 创建 trace 文件
	f, err := os.Create("trace.out")
	if err != nil {
		log.Fatalln("os.Create trace.out error", err)
	}
	defer f.Close()

	// 启动 trace goroutine
	err = trace.Start(f)
	if err != nil {
		log.Fatalln("trace.Start", err)
	}
	defer trace.Stop()

	// main
	log.Println("Hello, world")
}

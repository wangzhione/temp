package main

import (
	"fmt"
	"runtime/debug"
)

func main() {
	fmt.Println("你好")
}

// SafeGo 起一个不会 panic 的协程, 可以拿到错误信息
func SafeGo(fn func() error) <-chan error {
	result := make(chan error, 1)

	go func() {
		defer func() {
			if rec := recover(); rec != nil {
				result <- fmt.Errorf("panic: %+v, %s", rec, debug.Stack())
			}
		}()

		// 执行任务, 结果回写
		result <- fn()
	}()

	return result
}

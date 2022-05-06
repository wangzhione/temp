package main

import (
	"fmt"
	"runtime"
	"time"
)

func memoryLeaking() {
	type T struct {
		v [2]int
		t *T
	}

	var finalizer = func(t *T) {
		fmt.Println("finalizer called")
	}

	var x T

	// 此SetFinalizer函数调用将使x逃逸到堆上。
	runtime.SetFinalizer(&x, finalizer)

	// 下面这行将形成一个包含 x 和 y 的循环引用值组。
	// 这有可能造成 x 和 y 不可回收。
	// y 也逃逸到了堆上。

	var y T
	x.t = &y
	x.t, y.t = &y, &x
}

// go run leaking.go
// go build -gcflags="-m" leaking.go
func main() {
	memoryLeaking()

	runtime.GC()
	time.Sleep(time.Second)
}

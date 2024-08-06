package main

import (
	"fmt"
	"sync/atomic"
	"time"
)

func main() {
	value := atomic.Value{}

	value.Store(1)
	// value.Store("1")

	fmt.Printf("value = %+v\n", value.Load())

	go func() {
		*(*int)(nil) = 0 // not reached
	}()

	time.Sleep(time.Second)

	fmt.Println("main exit")
}

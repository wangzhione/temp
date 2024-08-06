package main

import (
	"fmt"
	"time"
)

func main() {
	ch := make(chan struct{})
	go func() {
		ch <- struct{}{}
	}()

	time.Sleep(time.Second)
	// ch 永远是 0, 0
	fmt.Printf("ch = %d, %d\n", len(ch), cap(ch))

	ch1 := make(chan struct{}, 2)
	ch1 <- struct{}{}
	fmt.Printf("ch1 = %d, %d\n", len(ch1), cap(ch1))
}

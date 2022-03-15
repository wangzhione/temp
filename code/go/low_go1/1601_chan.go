package main

import "fmt"

// build:
// go run 1601_chan.go
//
func main() {
	a := make(chan int)
	b := make(chan int)

	c := 1
	go func() {
		for i := 0; i < 2; i++ {
			select {
			case a <- 1:
				a = nil
			case b <- 2:
				b = nil
			}
		}

		// 局部变量修改, 不影响外部作用域. 值传递 a b c
		c = 3
	}()

	fmt.Println("c =", c)
	fmt.Println(<-a)
	fmt.Println(<-b)
	fmt.Println("c =", c)
}

/*
c = 1
1
2
c = 1
*/

package main

import (
	"context"
	"fmt"
	"unsafe"
)

var ch1 chan int
var ch2 chan int
var chs = []chan int{ch1, ch2}
var numbers = []int{1, 2, 3, 4, 5}

type ContextKey string

func main() {
	println("Hello, 世界")

	var ch3 = make(chan int, 1)
	ch3 <- 1

	select {
	case getChan(0) <- getNumber(2):
		fmt.Println("1th case is selected.")
	case getChan(1) <- getNumber(3):
		fmt.Println("2th case is selected.")
	default:
		fmt.Println("default!.")
	}

	fmt.Printf("todo size = %d\n", unsafe.Sizeof(context.TODO()))

	ctx := context.WithValue(context.TODO(), ContextKey("key"), "value")
	fmt.Printf("ctx = %+v\n", ctx)
	fmt.Printf("context.TODO() = %+v\n", context.TODO())

	err := ctx.Err()
	fmt.Printf("err = %+v\n", err)
}

func getNumber(i int) int {
	fmt.Printf("numbers[%d]\n", i)

	return numbers[i]
}

func getChan(i int) chan int {
	ch := chs[i]

	fmt.Printf("chs[%d] = %+v\n", i, ch)

	return ch
}

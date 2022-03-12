package main

import (
	"os"
	"runtime/debug"
	"runtime/pprof"

	"fmt"
)

// build:
// go run 91_stack.go
//
func main() {
	a(99)
	fmt.Printf("\n")
	b(100)
	fmt.Printf("\n")

	// 01 &^ 10 -> 01 & ^10 -> 01 & 01 -> 01
	c := 1 & ^2
	fmt.Printf("c = %d\n", c)

	c &^= 1 << 1
	fmt.Printf("c = %d\n", c)

	// https://docs.huihoo.com/go/golang.org/ref/spec/index.html#Arithmetic_operators
	// bit clear (AND NOT)
	// 如果运算符右侧数值的第 i 位为 1，那么计算结果中的第 i 位为 0；
	// 如果运算符右侧数值的第 i 位为 0，那么计算结果中的第 i 位为运算符左侧数值的第 i 位的值。
	c = 1
	// 01 &^= 11 -> 00
	c &^= 3
	fmt.Printf("c = %d\n", c)

	// 1 ^= 3 -> 1 ^ 3 = 01 ^ 11 = 10
	c = 1
	c ^= 3
	fmt.Printf("c = %d\n", c)
}

func a(num int) {
	debug.PrintStack()
}

func b(num int) {
	pprof.Lookup("goroutine").WriteTo(os.Stdout, 1)
}

// go run -gcflags="-N -l" 91_stack.go

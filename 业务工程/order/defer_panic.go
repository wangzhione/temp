package main

import "fmt"

func f(n int) (r int) {
	defer func() {
		r += n
		// recover()
	}()

	var f func()

	defer f()
	f = func() {
		r += 2
	}
	return n + 1
}

// go run defer_panic.go
func main() {
	fmt.Println(f(3))
}

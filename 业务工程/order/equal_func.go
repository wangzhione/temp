package main

import "fmt"

// build:
// go run equal_func.go
//
func main() {
	// # command-line-arguments
	// ./equal_func.go:9:22: invalid operation: func() {} == func() {} (func can only be compared to nil)
	fmt.Printf("%+v\n", func() {} == func() {})
}

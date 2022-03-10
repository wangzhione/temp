package main

import "fmt"

// build:
// go run 11_closure.go
// go tool compile -m=2 11_closure.go
// go tool compile -m=2 11_closure.go | grep capturing
//
func main() {
	a := 1
	b := 2
	go func() {
		fmt.Println(a, b)
	}()
	a = 99
}

// go tool compile -m=2 11_closure.go | grep capturing
/*
11_closure.go:11:2: main capturing by ref: a (addr=false assign=true width=8)
11_closure.go:12:2: main capturing by value: b (addr=false assign=false width=8)
*/

// https://iswade.github.io/go/go_tools/
// go tool compile -m=2
//   -m    print optimization decisions

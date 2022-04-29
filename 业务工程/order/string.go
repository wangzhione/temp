package main

import "fmt"

const s = "Go101.org" // len(s) == 9

// len(s) 是一个常量表达式, 但 len(s[:]) 却不是
var a byte = 1 << len(s) / 128
var b byte = 1 << len(s[:]) / 128

func main() {
	fmt.Println(a, b)

	m := 9
	c := byte(1) << m
	fmt.Println(c)

	fmt.Println(multiple)
}

var multiple = `
111 
122
222
`[1:]

package main

import (
	"fmt"
	"strings"
	"unicode/utf8"
)

// build:
// go run 50_string.go
//
func main() {
	var a = "hello, world"
	for i := 0; i < len(a); i++ {
		fmt.Printf("%x ", a[i])
	}
	fmt.Printf("\nlen(a) = %d\n\n", len(a))

	var b = "Go 语言"
	for i := 0; i < len(b); i++ {
		fmt.Printf("%x ", b[i])
	}
	fmt.Printf("\nlen(b) = %d, rune len(b) = %d\n\n", len(b), len([]rune(b)))

	for index, value := range b {
		fmt.Printf("%#U starts at byte position %d\n", value, index)
	}
	fmt.Printf("\n")

	const hoge = "Go 语言"
	for i, w := 0, 0; i < len(hoge); i += w {
		var value rune
		value, w = utf8.DecodeRuneInString(hoge[i:])
		fmt.Printf("%#U starts at byte position %d\n", value, i)
	}
	fmt.Printf("len(hoge) = %d, rune len(hoge) = %d\n\n", len(hoge), utf8.RuneCountInString(hoge))

	// 会分割空数组
	fileds := strings.Split("1|||23||34||", "|")
	for index, filed := range fileds {
		fmt.Printf("index = %d, filed = [%s], len = %d, cap = %d\n", index, filed, len(filed), cap(fileds))
	}
}

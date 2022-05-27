package main

import "fmt"

// go run package.go
func main() {
	type Key1 int
	type Key2 int

	var key1 interface{} = Key1(1)
	var key2 interface{} = Key2(1)

	// main.Key1:1 != main.Key2:1
	if key1 == key2 {
		fmt.Printf("%T:%v == %T:%v\n", key1, key1, key2, key2)
	} else {
		fmt.Printf("%T:%v != %T:%v\n", key1, key1, key2, key2)
	}
}

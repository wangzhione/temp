package main

import "fmt"

// 1. 指向栈上对象的指针不能分配到堆上
// 2. 指向栈上对象的指针，其生命周期不能长于该对象

// go run 003_add.go
// go run -gcflags '-m -l' 003_add.go
func main() {
	fmt.Println(Add(1, 2))
	fmt.Println(Add2(1, 2))
}

func Add(a, b int) (add *int) {
	*add = a + b
	return add
}

func Add2(a, b int) *int {
	add := 0
	add = a + b
	return &add
}

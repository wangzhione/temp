package main

func main() {
	println(test1(), test2(), test3())
}

func test1() (a int) {
	a = 9
	defer func() { a = 99 }()
	return a
}

func test2() int {
	a := 9
	defer func() { a = 99 }()
	return a
}

func test3() int {
	a := 9
	defer func(pa *int) { *pa = 99 }(&a)
	return a
}

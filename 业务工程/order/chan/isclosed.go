package main

import "fmt"

func IsClosed(ch <-chan int) bool {
	select {
	case <-ch:
		return true
	default:
	}
	return false
}

// go run isclosed.go
func main() {
	c := make(chan int)
	fmt.Println(IsClosed(c)) // false
	close(c)
	fmt.Println(IsClosed(c)) // true

	// 如果我们能够保证从不会向一个通道发送数据，那么有一个简单的方法来判断此通道是否已经关闭
	b := make(chan int, 1)
	b <- 1
	fmt.Println(IsClosed(b)) // false
	close(b)
	fmt.Println(IsClosed(b)) // true
}

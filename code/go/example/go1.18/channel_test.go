package main

import (
	"fmt"
	"testing"
)

func Test_channel_1(t *testing.T) {
	c := make(chan int, 1)
	for done := false; !done; {
		select {
		default:
			print(1)
			done = true
		case <-c:
			print(2)
			c = nil
		case c <- 1:
			print(3)
		}
	}
}

/*
 321

channel	nil	空的	非空非满	满了
往channel发送数据	阻塞	发送成功	发送成功	阻塞
从channel接收数据	阻塞	阻塞	接收成功	接收成功
关闭channel	panic	关闭成功	关闭成功	关闭成功
*/

func f() bool {
	return false
}

func Test_quiz_switch(t *testing.T) {
	switch f(); {
	case true:
		println(1)
	case false:
		println(0)
	default:
		println(-1)
	}
}

/*
 类比下面这个语法
 switch f(); true {

 }

 不推荐写法
*/

// Foo prints and returns n.
func Foo(n int) int {
	fmt.Println(n)
	return n
}

func Test_switch_Foo(t *testing.T) {
	switch Foo(2) {
	case Foo(1), Foo(2), Foo(3):
		fmt.Println("First case")
		fallthrough
	case Foo(4):
		fmt.Println("Second case")
		fallthrough
	case Foo(2):
		fmt.Println("third case")
	case Foo(5):
		fmt.Println("fourth case")
	}
}

/*
2
1
2
First case
Second case
third case

按照顺序计算 case 操作. 一但命中, 后续就不再执行 case 上语句
*/

func Test_inc(t *testing.T) {
	a := 1
	fmt.Println(a)
	// fmt.println(a++)
}

func Test_switch_2(t *testing.T) {
	switch 5 {
	case 10:
		fmt.Println("1 10")
		fallthrough
	case 9:
		fmt.Println("2  9")
		fallthrough
	case 2:
		fmt.Println("3  2")
	case 1:
		fmt.Println("4  1")
	case 5:
		fmt.Println("5  5")
	}
}

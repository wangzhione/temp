package main

import (
	"fmt"
	"testing"
)

func Test_quiz_1_0(t *testing.T) {
	a := make([]int, 0, 4)
	b := append(a, 1)    // b=[1], b指向的底层数组的首元素为1，但是a的长度和容量不变
	c := append(a, 2)    // a的长度还是0，c=[2], c指向的底层数组的首元素变为2
	fmt.Println(a, b, c) // [] [2] [2]
}

func Test_quiz_1(t *testing.T) {
	a := [...]int{0, 1, 2, 3}

	x := a[:1]
	fmt.Println(x, "len(x) =", len(x), "cap(x) =", cap(x))

	y := a[2:]
	fmt.Println(y, "len(y) =", len(y), "cap(y) =", cap(y))

	d := a[1:]
	fmt.Println(d, "len(d) =", len(d), "cap(d) =", cap(d))

	x = append(x, y...)
	x = append(x, y...)
	fmt.Println(a, x)
}

/*
 a := [...]{0, 1, 2, 3}
 x := a[:1] => x := { 0 } slice 指向 a[0:1] 数组数组
 y := a[2:] => y := { 2, 3 } slice 指向 a[2:] 数组

 x = append(x, y...)
 x = { 0, 2, 3 } 地址重叠 则 a := [...]{ 0, 2, 3, 3 }, 此刻 y 为 { 3, 3 }
 x = append(x, y...)
 x = {0, 2, 3, 2, 3} 此刻数据过多, 重新申请内存 a := [...]{ 0, 2, 3, 3 } 不变

 所以
 [0, 2, 3, 3] [0, 2, 3, 3, 3]
*/

func Test_quiz_2(t *testing.T) {
	a := []int{1, 2, 3}
	b := a[1:]
	copy(a, b)
	fmt.Println(a, b)
}

/*
 a := []int{1, 2, 3}
 b := a[1:] -> b := []int{2, 3} 和 a 内存重叠
 copy(a, b) copy 底层 memmove 所以有
 a[1] = b[1] = 3
 a[0] = b[0] = 2

 所以最终 a = [2, 3, 3], b 和 a 同内存, 所以是 [3, 3]
*/

func Test_quiz_3(t *testing.T) {
	a := []int{1, 2}
	b := append(a, 3)

	c := append(b, 4)
	d := append(b, 5)

	fmt.Println(a, b, c[3], d[3])
}

/*
 a := []int{1, 2} , len = 2 cap =2
 b := append(a, 3) => b := []int{1, 2, 3} len = 3, cap = 4

 c := append(b, 4) => c := []{1, 2, 3, 4} len = 4, cap = 4

 d := append(b, 5) => d := {1, 2, 3, 5} len = 4, cap = 4 和 b 和 c 共内存
 则 c[3] = 5

 最终
 [1, 2] [1, 2, 3] 5 5
*/

func Test_quiz_4(t *testing.T) {
	s := []int{1, 2}
	fmt.Println(len(s), cap(s))
	/*
		s = append(s, 4)
		s = append(s, 4)
		s = append(s, 4)

		这种 是 5 8
	*/
	s = append(s, 4, 5, 6)
	fmt.Println(len(s), cap(s))
}

/*
 s := []int{1, 2} => len = 2 , cap = 2

 s = append(s, 4, 5, 6)
 append 4 => len = 3, cap = 4
 append 5 => len = 4, cap = 4
 append 6 => len = 5, cap = 8

 上面分析有问题, 因为 append 比想象复杂, 会对这类 append slice 做特殊处理.

 func growslice(et *_type, old slice, cap int) slice {}

 可以看到, 比较复杂. 目前记住 这个值 >= len(s) 就好
*/

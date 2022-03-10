package main

import (
	"fmt"
	"math/big"
)

// build:
// go run 22_big.go
//
func main() {
	a := big.NewInt(0)
	b := big.NewInt(1)

	var limit big.Int

	/*
		如下例计算出第一个大于10^{99}的斐波那契序列的值。
		在该示例中，使用 big.NewInt 函数初始化 big.Int，
		使用 big.Exp 函数计算 10^{99} 的大小，
		使用 big.Cmp 函数比较大整数的值，
		使用 big.Add 函数计算大整数的加法。
	*/

	limit.Exp(big.NewInt(10), big.NewInt(99), nil)
	for a.Cmp(&limit) < 0 {
		a.Add(a, b)
		a, b = b, a
	}
	fmt.Println(a)

	var x1, y1 float64 = 10, 3
	z1 := x1 / y1
	fmt.Println(x1, y1, z1)

	x2, y2 := big.NewFloat(10), big.NewFloat(3)
	x2.SetPrec(100)
	y2.SetPrec(100)
	z2 := new(big.Float).Quo(x2, y2)
	fmt.Println(x2, y2, z2)

	x, y := big.NewRat(1, 2), big.NewRat(2, 3)
	z := new(big.Rat).Mul(x, y)
	fmt.Println("z: ", z)
}

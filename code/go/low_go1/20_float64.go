package main

import "fmt"

// build:
// go run 20_float64.go
//
func main() {
	var f1 float64 = 0.3
	var f2 float64 = 0.6
	var f3 = f1 + f2

	// 最小测试单元

	/*
	 shouldRoundUp -> RoundUp 0.8999999 + 进了 1 -> 0.900000 (其实是 0.999999 取小数第一位)
	*/
	fmt.Printf("%f", f3) //0.900000

	fmt.Println(f3) //0.8999999999999999\n

	// %F 和 %f 相同意思. synonym for %f
	fmt.Printf("f1 = %f, f2 = %f, f1+f2 = %f\n", f1, f2, f1+f2)
	fmt.Println(f1, f2, f1+f2)
}

/*
// fmtFloat formats a float. The default precision for each verb
// is specified as last argument in the call to fmt_float.
func (p *pp) fmtFloat(v float64, size int, verb rune) {
	switch verb {
	case 'v':
		p.fmt.fmtFloat(v, size, 'g', -1)
	case 'b', 'g', 'G', 'x', 'X':
		p.fmt.fmtFloat(v, size, verb, -1)
	case 'f', 'e', 'E':
		p.fmt.fmtFloat(v, size, verb, 6)
	case 'F':
		p.fmt.fmtFloat(v, size, 'f', 6)
	default:
		p.badVerb(verb)
	}
}
*/

// 'F' -> 'f'

/*
// /usr/local/go/src/fmt/print.go
// func (p *pp) printArg(arg interface{}, verb rune) {

	case float32:
		p.fmtFloat(float64(f), 32, verb)
	case float64:
		p.fmtFloat(f, 64, verb)
*/

// float32 -> float64

package main

import "log"

// go 1.18 特性学习
// go run main.go
// go build
func main() {
	var ai, bi, ci int
	var af, bf, cf float32

	ai = 1
	bi = 2
	ci = min(ai, bi)
	log.Printf("ai = %d, bi = %d, ci = %d\n", ai, bi, ci)

	af = 2.0
	bf = 3.0
	cf = min(af, bf)
	log.Printf("af = %f, bf = %f, cf = %f\n", af, bf, cf)
}

type OrderType interface {
	~int | ~float32
}

func min[P OrderType](x, y P) P {
	if x < y {
		return x
	}
	return y
}

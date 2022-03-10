package main

import (
	"fmt"
	"math"
)

// TODO: move elsewhere?
type floatInfo struct {
	mantbits uint
	expbits  uint
	bias     int
}

var float32info = floatInfo{23, 8, -127}
var float64info = floatInfo{52, 11, -1023}

// x=(−1)^S×(1.M)×2^e
// e = E − 1023

// build:
// go run 21_float32.go
//
func main() {
	var number float32 = 0.085

	fmt.Printf("Starting Number: %f\n\n", number)

	bits := math.Float32bits(number)
	binary := fmt.Sprintf("%.32b", bits)

	fmt.Printf("Number binary: %v\n\n", binary)
	fmt.Printf("Bit Pattern: %s | %s %s | %s %s %s %s %s %s\n\n",
		binary[0:1],
		binary[1:5], binary[5:9],
		binary[9:12], binary[12:16], binary[16:20],
		binary[20:24], binary[24:28], binary[28:32])

	bias := 127
	sign := bits & (1 << 31)
	exponentRaw := int(bits >> 23)
	exponent := exponentRaw - bias
	var mantissa float64
	for index, bit := range binary[9:32] {
		if bit == 49 {
			position := index + 1
			bitValue := math.Pow(2, float64(position))
			fractional := 1 / bitValue
			mantissa = mantissa + fractional
		}
	}

	value := (1 + mantissa) * math.Pow(2, float64(exponent))
	fmt.Printf("Sign: %d Exponent: %d (%d) Mantissa: %f Value: %f\n\n",
		sign,
		exponentRaw,
		exponent,
		mantissa,
		value)
}

func isInt(bits uint32, bias int) {
	exponent := int(bits>>23) - bias - 23
	coefficient := (bits&(1<<23) - 1) | (1 << 23)
	intTest := coefficient & (1<<uint32(-exponent) - 1)

	fmt.Printf("\nEcponent: %d Coefficient: %d IntTest: %d\n",
		exponent,
		coefficient,
		intTest)
	if exponent < -23 {
		fmt.Printf("Not INTEGER\n")
		return
	}
	if exponent < 0 && intTest != 0 {
		fmt.Printf("NOT INTEGER\n")
		return
	}
	fmt.Printf("INTEGER\n")
}

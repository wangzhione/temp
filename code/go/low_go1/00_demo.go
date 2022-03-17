package main

import (
	"fmt"
	"time"
)

// build:
// go run 00_demo.go
//
// func main() {
// 	i := 55           // int
// 	j := 67.8         // float64
// 	sum := i + int(j) // j is converted to int

// 	println(i, j, sum)
// }

// func main() {
// 	i := 0

// 	quit := make(chan struct{}, 1)
// 	go func(ptr *int) {
// 		select {
// 		case <-quit:
// 			fmt.Println("go func quit", *ptr)
// 			return
// 		default:
// 			// ...
// 			// ..
// 			// .
// 			for {
// 				*ptr++
// 				fmt.Println("go func for loop", *ptr)
// 				time.Sleep(100 * time.Millisecond)
// 			}
// 		}
// 		fmt.Println("go func for loop exit", *ptr)
// 	}(&i)

// 	fmt.Println("main begin", i)

// 	time.Sleep(time.Second)

// 	// ... .. .
// 	quit <- struct{}{}

// 	fmt.Println("main exit", i)
// }

func main() {
	i := 0

	quit := make(chan struct{}, 1)
	go func(ptr *int) {
		defer fmt.Println("go func for loop exit", *ptr)

		for {
			select {
			case <-quit:
				fmt.Println("go func quit", *ptr)
				return
			default:
				// ...
				// ..
				// .
				*ptr++
				fmt.Println("go func for loop", *ptr)
				time.Sleep(100 * time.Millisecond)
			}
		}
	}(&i)

	fmt.Println("main begin", i)

	time.Sleep(time.Second)

	// ... .. .
	quit <- struct{}{}

	time.Sleep(time.Second)

	fmt.Println("main exit", i)
}

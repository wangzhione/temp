package main

import (
	"fmt"
	. "fmt"
	"unsafe"
)

// build:
// go run 70_slice.go
//
func main() {
	var slice2 []int = make([]int, 13)
	Printf("2 pointer = %p, sizeof(slice) = %d, len = %d, cap = %d\n", slice2, unsafe.Sizeof(slice2), len(slice2), cap(slice2))

	for i := 0; i < 12; i++ {
		slice2 = append(slice2, i)
	}
	Printf("2 pointer = %p, sizeof(slice) = %d, len = %d, cap = %d\n", slice2, unsafe.Sizeof(slice2), len(slice2), cap(slice2))

	var a int
	var b int64
	Printf("sizeof(int) = %d, sizeof(int64) = %d\n", unsafe.Sizeof(a), unsafe.Sizeof(b))

	slice3 := slice2[1:3]
	slice4 := slice3[1:3]
	Printf("slice2 = %p, slice3 = %p, slice4 = %p\n", slice2, slice3, slice4)
	Printf("3 pointer = %p, sizeof(slice) = %d, len = %d, cap = %d\n", slice3, unsafe.Sizeof(slice3), len(slice3), cap(slice3))
	Printf("4 pointer = %p, sizeof(slice) = %d, len = %d, cap = %d\n", slice4, unsafe.Sizeof(slice4), len(slice4), cap(slice4))

	slice2[2] = -1
	fmt.Printf("slice3[1] = %d, slice2[3] = %d, slice2[2] = %d\n", slice3[1], slice2[3], slice2[2])
	slice3 = append(slice3, 3)
	Printf("3 pointer = %p, sizeof(slice) = %d, len = %d, cap = %d\n", slice3, unsafe.Sizeof(slice3), len(slice3), cap(slice3))
	Printf("2 pointer = %p, sizeof(slice) = %d, len = %d, cap = %d\n", slice2, unsafe.Sizeof(slice2), len(slice2), cap(slice2))
	fmt.Printf("slice3[2] = %d, slice2[3] = %d\n", slice3[2], slice2[3])
}

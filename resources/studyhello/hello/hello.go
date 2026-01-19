package main

import (
	"fmt"
	"time"
)

func main() {
	fmt.Println("before")
	time.Sleep(1 * time.Second)
	fmt.Println("after")
}

package main

import (
	"fmt"
	"log"
	"os"
	"runtime/pprof"

	_ "net/http/pprof"
)

// build:
// go run 2100_pprof.go
//
func main() {
	f, err := os.Create("cpuProfile")
	if err != nil {
		log.Fatalln("os.Create cpuProfile error", err)
	}
	defer f.Close()

	err = pprof.StartCPUProfile(f)
	if err != nil {
		log.Fatalln("pprof.StartCPUProfile error", err)
	}
	defer pprof.StopCPUProfile()

	fmt.Println("Hello, World")
}

/*
top

top -cum

list Run

tree

web
*/

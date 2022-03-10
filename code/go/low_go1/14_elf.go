package main

import (
	"debug/elf"
	"fmt"
	"log"
)

// build:
// go run 14_elf.go
//
func main() {
	log.SetFlags(log.Lshortfile | log.Ldate | log.Lmicroseconds)

	f, err := elf.Open("14_elf")
	if err != nil {
		log.Fatal(err)
	}

	for index, section := range f.Sections {
		fmt.Printf("%2d ", index)
		fmt.Println(section)
	}

	err = f.Close()
	if err != nil {
		log.Fatal(err)
	}
}

// readelf -h 14_elf

// go build 14_elf.go & ./14_elf

// objdump -s -j .note.go.buildid 14_elf

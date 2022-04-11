package main

import (
	"log"
	"net/http"
)

func main() {
	resp, err := http.Get("http://www.google.com")
	if err != nil {
		log.Fatalln("http.Get http://www.google.com error", err)
	}
	defer resp.Body.Close()
}

package main

import (
	"fmt"
	"net/http"
	"time"
)

// build:
// go run 90_http.go
//
func main() {
	http.HandleFunc("/hello", timed(hello))
	http.ListenAndServe(":3000", nil)
}

func timed(f func(http.ResponseWriter, *http.Request)) func(http.ResponseWriter, *http.Request) {
	return func(w http.ResponseWriter, r *http.Request) {
		start := time.Now()
		f(w, r)
		end := time.Now()
		fmt.Println("The request took", end.Sub(start))
	}
}

func hello(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("你好. Hello, World!"))
}

package main

import (
	"fmt"
	"log"
	"net/http"
	"sync/atomic"
)

var (
	counter uint64
)

// CounterHandler 计数器 handle
type CounterHandler struct{}

func (CounterHandler) ServeHTTP(writer http.ResponseWriter, request *http.Request) {
	atomic.AddUint64(&counter, 1)
	fmt.Fprintln(writer, counter)
	fmt.Println(counter)
}

// go run 001_http_service_counter.go
func main() {
	server := &http.Server{
		Addr:    ":8080",
		Handler: CounterHandler{},
	}

	err := server.ListenAndServe()
	if err != nil {
		log.Fatalf("server.ListenAndServe error = %+v\n", err)
	}
}

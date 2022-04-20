package main

import (
	"context"
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"time"
)

var (
	ctx = context.Background()
)

// go run 002_http_shutdown.go
func main() {
	handler := http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		time.Sleep(2 * time.Second)
		fmt.Fprintln(w, "Hello, World!")
	})

	server := http.Server{
		Addr:    ":8080",
		Handler: handler,
	}

	// make sure idle connections returned
	processed := make(chan struct{})
	go func() {
		chs := make(chan os.Signal, 1)
		signal.Notify(chs, os.Interrupt)
		<-chs

		shutdownCtx, fcancel := context.WithTimeout(ctx, 3*time.Second)
		defer fcancel()

		err := server.Shutdown(shutdownCtx)
		if err != nil {
			log.Fatalf("server shutdown failed error = %+v\n", err)
		}
		log.Println("server gracefully shutdown")

		close(processed)
	}()

	err := server.ListenAndServe()
	if err != http.ErrServerClosed {
		log.Fatalf("server not gracefully shutdown error = %+v", err)
	}

	// waiting for goroutine above processd
	<-processed
}

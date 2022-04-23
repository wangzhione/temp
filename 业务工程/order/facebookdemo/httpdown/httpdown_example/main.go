package main

import (
	"facebookdemo/httpdown"
	"flag"
	"fmt"
	"net/http"
	"os"
	"time"
)

func handler(w http.ResponseWriter, r *http.Request) {
	duration, err := time.ParseDuration(r.FormValue("duration"))
	if err != nil {
		http.Error(w, err.Error(), 400)
	}
	fmt.Fprintf(w, "going to sleep %s with pid %d\n", duration, os.Getpid())
	w.(http.Flusher).Flush()
	time.Sleep(duration)
	fmt.Fprintf(w, "slept %s with pid %d\n", duration, os.Getpid())
}

func main() {
	server := &http.Server{
		Addr:    "127.0.0.1:8080",
		Handler: http.HandlerFunc(handler),
	}
	h := &httpdown.HTTP{
		StopTimeout: 10 * time.Second,
		KillTimeout: 1 * time.Second,
	}

	flag.StringVar(&server.Addr, "addr", server.Addr, "http address")
	flag.DurationVar(&h.StopTimeout, "stop-timeout", h.StopTimeout, "stop timeout")
	flag.DurationVar(&h.KillTimeout, "kill-timeout", h.KillTimeout, "kill timeout")
	flag.Parse()

	if err := httpdown.ListenAndServe(server, h); err != nil {
		panic(err)
	}
}

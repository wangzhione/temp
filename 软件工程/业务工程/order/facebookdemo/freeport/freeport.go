// Package freeport provides an API to find a free port to bind to.
package freeport

import (
	"net"
	"strconv"
)

// Get a free port int, err error
func Get() (int, error) {
	listener, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		return 0, err
	}
	defer listener.Close()

	addr := listener.Addr().String()
	_, protString, err := net.SplitHostPort(addr)
	if err != nil {
		return 0, err
	}

	return strconv.Atoi(protString)
}

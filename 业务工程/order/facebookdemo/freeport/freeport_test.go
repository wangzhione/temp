package freeport

import (
	"net"
	"strconv"
	"testing"
)

func TestGet(t *testing.T) {
	port, err := Get()
	if err != nil {
		t.Fatalf("Got err from Get: %+v", err)
	}

	if port == 0 {
		t.Fatal("Got port 0")
	}

	t.Logf("port = %d", port)
}

func TestGetIsClosed(t *testing.T) {
	port, err := Get()
	if err != nil {
		t.Fatalf("Got err from Get: %+v", err)
	}

	listener, err := net.Listen("tcp", "127.0.0.1:"+strconv.Itoa(port))
	if err != nil {

		t.Fatalf("Got err from net.Listen: %+v", err)
	}
	defer listener.Close()

	t.Logf("net.Listen 127.0.0.1:" + strconv.Itoa(port))
}

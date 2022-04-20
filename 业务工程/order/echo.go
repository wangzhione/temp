package main

import (
	"io"
	"log"
	"net"
)

// echo service run : go run echo.go    | Ctrl + C
// echo client run  : telnet -6 :: 9527 | Ctrl + ] + q Enter
func main() {
	listen, err := net.Listen("tcp", ":9527")
	if err != nil {
		log.Fatalf("net.Listen error = %+v\n", err)
	}

	for {
		conn, err := listen.Accept()
		if err != nil {
			log.Fatalf("listen.Accept error = %+v", err)
		}

		// start a goroutine to handle the new connection
		go HandleConn(conn)
	}
}

func HandleConn(conn net.Conn) {
	defer conn.Close()

	packet := make([]byte, 1024)
	for {
		// block here if socket is not available for reding data packet.
		n, err := conn.Read(packet)
		if err != nil {
			if err == io.EOF {
				// 对方发起了 FIN 码, 我们需要处理完相关逻辑再关闭.
				log.Println("Success closed")
				break
			}

			log.Printf("read socket error = %+v\n", err)
			return
		}

		// echo
		total := 0
		for total < n {
			// same as above, block here if socket is not available for writing.
			x, err := conn.Write(packet[total:n])
			if err != nil {
				log.Printf("write socket error = %+v\n", err)
				return
			}
			total += x
		}
	}
}

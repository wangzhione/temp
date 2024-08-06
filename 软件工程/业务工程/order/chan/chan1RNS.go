package main

import (
	"log"
	"math/rand"
	"sync"
	"time"
)

// 情形二：
// 一个接收者和N个发送者，此唯一接收者通过关闭一个额外的信号通道来通知发送者不要在发送数据了

// go run chan1RNS.go
func main() {
	rand.Seed(time.Now().UnixNano())

	const Max = 100000
	const NumSenders = 1000

	var group sync.WaitGroup
	group.Add(1)

	dataCh := make(chan int)

	// stopCh 是一个额外的信息通道.
	// 他的发送者为 dataCh 数据通道的接收者. 他的接收者为 dataCh 数据通道的发送者
	stopCh := make(chan struct{})

	// 发送者
	for i := 0; i < NumSenders; i++ {
		go func() {
			for {
				// 第一个尝试接收用来让此发送者协程尽早退出. 对于特定例子, 此 select 代码块并非必须
				select {
				case <-stopCh:
					return
				default:
				}

				// 即使 stopCh 已经关闭, 此第二个 select 代码块中的第一个分支仍很有可能若干个
				// 循环依然不会被选中.
				select {
				case <-stopCh:
					return
				case dataCh <- rand.Intn(Max):
				}
			}
		}()
	}

	// 接收者
	go func() {
		defer group.Done()

		for value := range dataCh {
			if value == Max-1 {
				// 此唯一的接收者同时也是 stopCh 通道的唯一发送者
				close(stopCh)
				return
			}

			log.Println(value)
		}
	}()

	group.Wait()
}

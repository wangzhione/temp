package main

import (
	"log"
	"math/rand"
	"sync"
	"time"
)

/*
 情形一:
  M个接收者和一个发送者。发送者通过关闭用来传输数据的通道来传递发送结束信号
  这是最简单的一种情形。当发送者欲结束发送，让它关闭用来传输数据的通道即可。
*/

// go run chanMR1S.go
func main() {
	rand.Seed(time.Now().UnixNano())

	const Max = 100000
	const NumReceivers = 100

	wgReceivers := sync.WaitGroup{}
	wgReceivers.Add(NumReceivers)

	dataCh := make(chan int)

	// 消费者
	for i := 0; i < NumReceivers; i++ {
		go func() {
			defer wgReceivers.Done()

			// 接收数据直到通道 dataCh 已经关闭 && dataCh 缓冲队列变空
			for value := range dataCh {
				log.Println(value)
			}
		}()
	}

	// 生产者
	go func() {
		for {
			value := rand.Intn(Max)
			if value == 0 {
				// 此发送者是唯一可以安全关闭通道的选择
				close(dataCh)
				return
			}

			dataCh <- value
		}
	}()

	wgReceivers.Wait()
}

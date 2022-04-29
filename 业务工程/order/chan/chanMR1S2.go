package main

import (
	"log"
	"math/rand"
	"sync"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())

	const Max = 100000
	const NumReceivers = 100
	const NumThirdParties = 15

	group := sync.WaitGroup{}
	group.Add(NumReceivers)

	dataCh := make(chan int)
	closing := make(chan struct{}) // 信号通道
	closed := make(chan struct{})

	// 此 stop 函数可以被安全地多次调用
	stop := func() {
		select {
		case closing <- struct{}{}:
			<-closed
		case <-closed:
		}
	}

	// 一些第三方协程
	for i := 0; i < NumThirdParties; i++ {
		go func() {
			r := 1 + rand.Intn(3)
			time.Sleep(time.Duration(r) * time.Second)
			stop()
		}()
	}

	// 发送者
	go func() {
		defer func() {
			close(closed)
			close(dataCh)
		}()

		for {
			select {
			case <-closing:
				return
			default:
			}

			select {
			case <-closing:
				return
			case dataCh <- rand.Intn(Max):
			}
		}
	}()

	// 接收者
	for i := 0; i < NumReceivers; i++ {
		go func() {
			defer group.Done()

			for value := range dataCh {
				log.Println(value)
			}
		}()
	}

	group.Wait()
}

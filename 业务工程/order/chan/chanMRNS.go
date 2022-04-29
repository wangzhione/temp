package main

import (
	"log"
	"math/rand"
	"strconv"
	"sync"
	"time"
)

// 情形三：M个接收者和N个发送者。
// 它们中的任何协程都可以让一个中间调解协程帮忙发出停止数据传送的信号

// go run chanMRNS.go
func main() {
	rand.Seed(time.Now().UnixNano())

	const Max = 100000
	const NumReceivers = 10
	const NumSenders = 1000

	var group sync.WaitGroup
	group.Add(NumReceivers)

	dataCh := make(chan int)
	// stopCh 额外信号通道, 它的发送者为中间调节者. 它的接收者为 dataCh 通过所有者
	stopCh := make(chan struct{})

	// toStop 是一个用来通知中间调节者让其关闭信号通道 stopCh 的第二个信号通道.
	// 此第儿歌信号通道的发送者为 dataCh 数据通道的所有的发送者和接收者,
	// 他的接收者为中间调节者. 它必须为一个缓冲通道
	toStop := make(chan string, 1)

	var stoppedBy string

	// 中间调节者
	go func() {
		stoppedBy = <-toStop
		close(stopCh)
	}()

	// 发送者
	for i := 0; i < NumSenders; i++ {
		go func(id string) {
			for {
				value := rand.Intn(Max)
				if value == 0 {
					// 为了防止阻塞, 这里使用一个尝试, 发送操作来向中间调节者发送信号
					select {
					case toStop <- "发送者#" + id:
					default:
					}
					return
				}

				// 此处的尝试接收操作, 让发送协程尽早退出
				select {
				case <-stopCh:
					return
				default:
				}

				// 即使 stopCh 已经关闭, 如果这个 select 代码块中第二个分支的发送操作是非阻塞
				// 则第一个分支仍然可能在若干个循环内不会被选中. 则上面代码是必须的
				select {
				case <-stopCh:
					return
				case dataCh <- value:
				}
			}
		}(strconv.Itoa(i))
	}

	// 接收者
	for i := 0; i < NumReceivers; i++ {
		go func(id string) {
			defer group.Done()

			for {
				// 和发送者协程一样, 此处尝试接收操作是为了让此协程快速退出
				select {
				case <-stopCh:
					return
				default:
				}

				select {
				case <-stopCh:
					return
				case value := <-dataCh:
					if value == Max-1 {
						// w为了防止阻塞, 这里使用一个尝试发送操作
						select {
						case toStop <- "接收者#" + id:
						default:
						}
						return
					}

					log.Println(value)
				}
			}
		}(strconv.Itoa(i))
	}

	group.Wait()
	log.Println("被" + stoppedBy + "终止了")
}

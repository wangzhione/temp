# Go 语言底层原理剖析

https://github.com/golang/go

https://read.douban.com/reader/ebook/336450115/?dcs=bookshelf

https://draveness.me/golang/

https://github.com/aceld/golang

## 题目

### 1.与其他语言相比，使用 Go 有什么好处？

- 与其他作为学术实验开始的语言不同，Go 代码的设计是务实的。每个功能和语法决策都旨在让程序员的生活更轻松。

- Golang 针对并发进行了优化，并且在规模上运行良好。

- 由于单一的标准代码格式，Golang 通常被认为比其他语言可读性标准更加统一和具体。

- 自动垃圾收集明显比 Java 或 Python 更有效，因为它与程序同时执行。

### 2.Golang 使用什么数据类型？

- Interface

- Method

- Channel

- Function

- Pointer

- Map

- Slice

- Array

- String

- Bool

- Integer / Float

### 3.Go 程序中的包是什么？

包 (pkg) 是 Go 工作区中包含 Go 源文件或其他包的目录。源文件中的每个函数、变量和类型都存储在链接包中。每个 Go 源文件都属于一个包，该包在文件顶部使用以下命令声明：

```Go
package [package name]
```

您可以使用以下方法导入和导出包以重用导出的函数或类型：

```GO
import [package name]
```

Golang 的标准包上百个. 例如其中 fmt 包, 包含格式化和打印功能，如 fmt.Println().

### 4、Go 支持什么形式的类型转换？

将整数和浮点数互相转换。 Go 支持显式类型转换部分常量隐式转换以满足其严格的类型要求。

```GO
i := 55           // int
j := 67.8         // float64
sum := i + int(j) // j is converted to int
```

### 5、什么是 Goroutine？你如何停止它？

一个 Goroutine 是一个函数或方法执行同时旁边其他任何够程采用了特殊的 Goroutine 线程。Goroutine 线程比标准线程更轻量级，大多数 Golang 程序同时使用数千个 Goroutine。

要创建 Goroutine，请 go 在函数声明之前添加关键字。

```Go
go f(x, y, z) 
```

您可以通过向 Goroutine 发送一个信号通道来停止它。Goroutines 只能在被告知检查时响应信号，因此您需要在逻辑位置（例如 for 循环顶部）包含检查。

```Go
package main

func main() {
	i := 0

	quit := make(chan struct{}, 1)
	go func(ptr *int) {
		select {
		case <-quit:
			fmt.Println("go func quit", *ptr)
			return
		default:
			// ...
			// ..
			// .
			for {
				*ptr++
				fmt.Println("go func for loop", *ptr)
				time.Sleep(100 * time.Millisecond)
			}
		}
		fmt.Println("go func for loop exit", *ptr)
	}(&i)

	fmt.Println("main begin", i)

	time.Sleep(time.Second)

	// ... .. .
	quit <- struct{}{}

	fmt.Println("main exit", i)
}
```

但其实是无法停止的, 例如上面这种除非 main goroutine exit, 否则就泄露了. 当然也可以特别改造.

```Go
package main

func main() {
	i := 0

	quit := make(chan struct{}, 1)
	go func(ptr *int) {
		defer fmt.Println("go func for loop exit", *ptr)
		
		for {
			select {
			case <-quit:
				fmt.Println("go func quit", *ptr)
				return
			default:
				// ...
				// ..
				// .
				*ptr++
				fmt.Println("go func for loop", *ptr)
				time.Sleep(100 * time.Millisecond)
			}
		}
	}(&i)

	fmt.Println("main begin", i)

	time.Sleep(time.Second)

	// ... .. .
	quit <- struct{}{}

	time.Sleep(time.Second)

	fmt.Println("main exit", i)
}
```

### 6、 如何在运行时检查变量类型？

类型开关(Type Switch)是在运行时检查变量类型的最佳方式。类型开关按类型而不是值来评估变量。每个 Switch 至少包含一个 case 用作条件语句，如果没有一个 case 为真，则执行 default。

### 7、Go 两个接口之间可以存在什么关系？

如果两个接口有相同的方法列表，那么他们就是等价的，可以相互赋值。如果接口 A 的方法列表被包含于接口 B 的方法列表的自己，那么接口 B 可以赋值给接口 A。接口查询是否成功，要在运行期才能够确定。

### 8、Go 当中同步锁有什么特点？作用是什么

当一个 Goroutine（协程）获得了 Mutex 后，其他 Goroutine（协程）就只能乖乖的等待，除非该 Goroutine 释放了该 Mutex。RWMutex 在读锁占用的情况下，会阻止写，但不阻止读 RWMutex。 在写锁占用情况下，会阻止任何其他 Goroutine（无论读和写）进来，整个锁相当于由该 Goroutine 独占 同步锁的作用是保证资源在使用时的独有性，不会因为并发而导致数据错乱，保证系统的稳定性。

### 题目：9、Go 语言当中 Channel（通道）有什么特点，需要注意什么？

- 如果给一个 nil 的 channel 发送数据，会造成永远阻塞。
- 如果从一个 nil 的 channel 中接收数据，也会造成永久阻塞。
- 给一个已经关闭的 channel 发送数据， 会引起 panic
- 从一个已经关闭的 channel 接收数据， 如果缓冲区中为空，则返回一个零值。

### 10、Go 语言当中 Channel 缓冲有什么特点？

无缓冲的 channel 是同步的，而有缓冲的 channel 是非同步的。

### 11、Go 语言中 cap 函数可以作用于哪些内容？

可以作用于的类型有：

- array(数组)
- slice(切片)
- channel(通道)

### 12、Go Convey 是什么？一般用来做什么？

- go convey是一个支持 Golang 的单元测试框架
- go convey 能够自动监控文件修改并启动测试，并可以将测试结果实时输出到 Web 界面
- go convey 提供了丰富的断言简化测试用例的编写

## 困难题目

### 6、GMP 中 hand off 机制

hand off 的场景是 syscall

当本线程 M 因为 G 进行的系统调用阻塞时，线程释放绑定的 P，把 P 转移给其他空闲的 M 执行。 

细节：当发生上线文切换时，需要对执行现场进行保护，以便下次被调度执行 时进行现场恢复。Go 调度器 M 的栈保存在 G 对象上，只需要将 M 所需要的寄存器（SP、PC 等）保存到 G 对象上就可以实现现场保护。当这些寄存器数据被保护起来，就随时可以做上下文切换了，在中断之前把现场保存起来。如果此时 G 任务还没有执行完，M 可以将任务重新丢到 P 的任务队列，等待下一次被调度执行。当再次被调度执行时，M 通过访问 G 的 vdsoSP、vdsoPC 寄存器进行现场 恢复（从上次中断位置继续执行）。

### 64. Go 语言如何实现可重入锁？

传统线程, 进程模型中可重入锁相比非可重入锁是和线程和进程绑定的. 例如通过线程 id 和 进程 id 来确定是否可重入. 

如果 Go 语言要实现可重入锁, 首先怎么定义这个可重入 ?

实现容易

- 记住持有锁的线程
- 统计重入的次数

Go语言的发明者认为，如果当你的代码需要重入锁时，那就说明你的代码有问题了，我们正常写代码时，从入口函数开始，执行的层次都是一层层往下的，如果有一个锁需要共享给几个函数，那么就在调用这几个函数的上面，直接加上互斥锁就好了，不需要在每一个函数里面都添加锁，再去释放锁。

### 66. go 内存操作也要处理 IO, 是如何处理的?

https://draveness.me/golang/docs/part3-runtime/ch07-memory/golang-memory-allocator/#713-%E5%86%85%E5%AD%98%E5%88%86%E9%85%8D
https://draveness.me/golang/docs/part3-runtime/ch07-memory/golang-stack-management/

### 101. 分布式锁知道哪些？用 channel 如何实现？

分布式锁, 常见有基于 MySQL, Redis, Redis Cluster, ZeeKeeper, Etcd 的.

业务还在单机就可以搞定的量级时，那么按照需求使用任意的单机锁方案就可以。

如果发展到了分布式服务阶段，但业务规模不大，qps很小的情况下，使用哪种锁方案都差不多。如果公司内已有可以使用的ZooKeeper、etcd或者Redis集群，那么就尽量在不引入新的技术栈的情况下满足业务需求。

业务发展到一定量级的话，就需要从多方面来考虑了。首先是你的锁是否在任何恶劣的条件下都不允许数据丢失，如果不允许，那么就不要使用Redis的setnx的简单锁。

对锁数据的可靠性要求极高的话，那只能使用etcd或者ZooKeeper这种通过一致性协议保证数据可靠性的锁方案。但可靠的背面往往都是较低的吞吐量和较高的延迟。需要根据业务的量级对其进行压力测试，以确保分布式锁所使用的etcd或ZooKeeper集群可以承受得住实际的业务请求压力。需要注意的是，etcd和Zookeeper集群是没有办法通过增加节点来提高其性能的。要对其进行横向扩展，只能增加搭建多个集群来支持更多的请求。这会进一步提高对运维和监控的要求。多个集群可能需要引入proxy，没有proxy那就需要业务去根据某个业务id来做分片。如果业务已经上线的情况下做扩展，还要考虑数据的动态迁移。这些都不是容易的事情。

在选择具体的方案时，还是需要多加思考，对风险早做预估。

https://chai2010.cn/advanced-go-programming-book/ch6-cloud/ch6-02-lock.html

https://mp.weixin.qq.com/s/lrSQBK-Kihkj6994kQFpUQ#at

### 102. 集群用 channel 如何实现分布式锁？

@see 03_redis_lock

## 后记

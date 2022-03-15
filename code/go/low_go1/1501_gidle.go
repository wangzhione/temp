package main

// build:
// go run 1501_gidle.go
//
func main() {

}

/*
 _Gidle :
	为协程刚开始创建时的状态, 当新创建的协程初始化后变为 _Gdead 状态.
	_Gdead 状态也是协程被销毁时的状态.

 _Grunnable :
	表示当前协程在运行队列中, 正在等待运行

 _Grunning :
	代表当前协程在运行队列中, 已经被分配给了逻辑处理器和线程.

 _Gwaiting :
	表示当前协程在运行时被锁定, 不能执行用户代码. 在垃圾回收及
	channel 通信时经常会遇到这种情况.

 _Gsyscall :
	代表当前协程正在执行系统调用.

 _Gpreempted :
 	是 Go 1.14 新加的状态, 代表协程 G 被强制抢占后的状态

 _Gcopystack :
	代表在进行协程扫描时发现需要扩容或缩小协程栈空间,
	将协程中的栈转移到新栈时的状态.

 _Gscan:

 _Gscanrunnable:

 _Gscanrunning:
 _
*/

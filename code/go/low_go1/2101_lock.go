package main

import "sync"

// build:
// go run 2101_lock.go
//
func main() {
	var counter int

	var l = NewLock()
	var wg sync.WaitGroup
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(ptr *int) {
			defer wg.Done()
			if l.TryLock() {
				*ptr++
				println("current counter =", *ptr)
				l.UnLock()
			} else {
				// 没有获取到锁
				println("lock false counter =", *ptr)
			}
		}(&counter)
	}
	wg.Wait()

	println("counter =", counter)
}

// Lock try lock
type Lock struct {
	c chan struct{}
}

func NewLock() (l Lock) {
	l.c = make(chan struct{}, 1)
	l.c <- struct{}{}
	return
}

// TryLock try lock, return lock result
func (l *Lock) TryLock() bool {
	select {
	case <-l.c:
		return true
	default:
	}
	return false
}

// UnLock unlock the try lock or lock
func (l *Lock) UnLock() {
	l.c <- struct{}{}
}

func (l *Lock) Lock() {
	<-l.c
}

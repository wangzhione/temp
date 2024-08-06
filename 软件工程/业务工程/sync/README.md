# go sync package

## 目录

```Bash
zhi@wang:/usr/local/go/src/sync$ tree -I '*test.go'
.
├── atomic
│   ├── asm.s
│   ├── doc.go
│   ├── race.s
│   └── value.go
├── cond.go
├── map.go
├── mutex.go
├── once.go
├── pool.go
├── poolqueue.go
├── runtime2.go
├── runtime2_lockrank.go
├── runtime.go
├── rwmutex.go
└── waitgroup.go

1 directory, 15 files
```

## atomic

### src/sync/atomic/doc.go

基于操作系统能力, 构建语言层面的 swap, compare-and-swap, add, load 和 store 原子操作. 

源码精华在头部注释

```Go
// Copyright 2011 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Package atomic provides low-level atomic memory primitives
// useful for implementing synchronization algorithms.
//
// These functions require great care to be used correctly.
// Except for special, low-level applications, synchronization is better
// done with channels or the facilities of the sync package.
// Share memory by communicating;
// don't communicate by sharing memory.
//
// The swap operation, implemented by the SwapT functions, is the atomic
// equivalent of:
//
//	old = *addr
//	*addr = new
//	return old
//
// The compare-and-swap operation, implemented by the CompareAndSwapT
// functions, is the atomic equivalent of:
//
//	if *addr == old {
//		*addr = new
//		return true
//	}
//	return false
//
// The add operation, implemented by the AddT functions, is the atomic
// equivalent of:
//
//	*addr += delta
//	return *addr
//
// The load and store operations, implemented by the LoadT and StoreT
// functions, are the atomic equivalents of "return *addr" and
// "*addr = val".
//
```

详情不用看: src/sync/atomic/asm.s -> src/runtime/internal/atomic 部分

### src/sync/atomic/value.go

```Go
// A Value provides an atomic load and store of a consistently typed value.
// The zero value for a Value returns nil from Load.
// Once Store has been called, a Value must not be copied.
//
// A Value must not be copied after first use.
type Value struct {
	v interface{}
}
```

Value 结构提供 '万能类型' interface{} Load 和 Store 原子操作.

首先看 Load 部分

```Go
// ifaceWords is interface{} internal representation.
type ifaceWords struct {
	typ  unsafe.Pointer
	data unsafe.Pointer
}

// Load returns the value set by the most recent Store.
// It returns nil if there has been no call to Store for this Value.
func (v *Value) Load() (x interface{}) {
	vp := (*ifaceWords)(unsafe.Pointer(v))
	typ := LoadPointer(&vp.typ)
	if typ == nil || uintptr(typ) == ^uintptr(0) {
		// First store not yet completed.
		return nil
	}
	data := LoadPointer(&vp.data)
	xp := (*ifaceWords)(unsafe.Pointer(&x))
	xp.typ = typ
	xp.data = data
	return
}
```

要点有两个

```Go
1. 
ifaceWords is interface{} internal representation.

2. 
if typ == nil || uintptr(typ) == ^uintptr(0) {
    // First store not yet completed.
    return nil
}

其中 ^uintptr(0) 是 Value Store 的中间状态, 这里当 nil 返回.
```

再看 Store 部分

```Go
// Store sets the value of the Value to x.
// All calls to Store for a given Value must use values of the same concrete type.
// Store of an inconsistent type panics, as does Store(nil).
func (v *Value) Store(x interface{}) {
	if x == nil {
		panic("sync/atomic: store of nil value into Value")
	}
	vp := (*ifaceWords)(unsafe.Pointer(v))
	xp := (*ifaceWords)(unsafe.Pointer(&x))
	for {
		typ := LoadPointer(&vp.typ)
		if typ == nil {
			// Attempt to start first store.
			// Disable preemption so that other goroutines can use
			// active spin wait to wait for completion; and so that
			// GC does not see the fake type accidentally.
			runtime_procPin()
			if !CompareAndSwapPointer(&vp.typ, nil, unsafe.Pointer(^uintptr(0))) {
				runtime_procUnpin()
				continue
			}
			// Complete first store.
			StorePointer(&vp.data, xp.data)
			StorePointer(&vp.typ, xp.typ)
			runtime_procUnpin()
			return
		}
		if uintptr(typ) == ^uintptr(0) {
			// First store in progress. Wait.
			// Since we disable preemption around the first store,
			// we can wait with active spinning.
			continue
		}
		// First store completed. Check type and overwrite data.
		if typ != xp.typ {
			panic("sync/atomic: store of inconsistently typed value into Value")
		}
		StorePointer(&vp.data, xp.data)
		return
	}
}

// Disable/enable preemption, implemented in runtime.
func runtime_procPin()
func runtime_procUnpin()
```

这里要点有三个

```Go
1. 
// Attempt to start first store.
// Disable preemption so that other goroutines can use
// active spin wait to wait for completion; and so that
// GC does not see the fake type accidentally.
runtime_procPin()

禁止当前 goroutine 被抢占, 等待 CPU 自旋完成.

详情看: src/runtime/proc.go 中 sync_runtime_procPin

2.
if !CompareAndSwapPointer(&vp.typ, nil, unsafe.Pointer(^uintptr(0))) {
    runtime_procUnpin()
    continue
}

如果 vp.type == nil 那么让 vp.typ = unsafe.Pointer(^uintptr(0)) 临时状态, 并返回 true. 
如果 vp.type != nil 那么返回 false, 进入 if 分支.

3. 
// First store completed. Check type and overwrite data.
if typ != xp.typ {
    panic("sync/atomic: store of inconsistently typed value into Value")
}

严格的 typ  unsafe.Pointer 校验. Value 只能被相同类型 interface{} Load 和 Store. 

举例: 

value := atomic.Value{}

value.Store(100)    ok  
value.Store("1")    panic | 不允许 int typ Store string typ
```

## src/sync/mutex.go


package main

import (
	"container/list"
	"sync"
)

// go 内联优化  : https://cloud.tencent.com/developer/article/1861199
// go LRU caceh : https://www.cnblogs.com/life2refuel/p/15057872.html
//

type entry struct {
	key, value interface{}
}

// CacheLUR
type CacheLRU struct {
	m    sync.Mutex                    // 因为 Get 触发热度更新操作, 所以只能走互斥
	cap  int                           // capacity 容量 -1 (or <= 0) 标识不限制
	list *list.List                    // entry container list
	data map[interface{}]*list.Element // 数据池子
}

// NewCacheLRU new CacheLRU return struct pointer
func NewCacheLRU(cap int) *CacheLRU {
	return &CacheLRU{
		cap:  cap,
		list: list.New(),
		data: make(map[interface{}]*list.Element),
	}
}

func (c *CacheLRU) Get(key interface{}) (value interface{}, ok bool) {
	c.m.Lock()
	defer c.m.Unlock()

	element, ok := c.data[key]
	if ok {
		// 获取指定缓存和值
		value = element.Value.(*entry).value
		// 调整 element 热度
		c.list.MoveToFront(element)
	}
	return
}

func (c *CacheLRU) Delete(key interface{}) {
	c.m.Lock()
	defer c.m.Unlock()

	element, ok := c.data[key]
	if ok {
		delete(c.data, key)
		c.list.Remove(element)
	}
}

func (c *CacheLRU) Put(key, value interface{}) {
	c.m.Lock()
	c.m.Unlock()

	element, ok := c.data[key]
	if ok {
		// 结点存在, 更新结点
		element.Value.(*entry).value = value
		// 调整 element 热度
		c.list.MoveToFront(element)
		return
	}

	// 容量不够走删除逻辑
	if c.cap > 0 && c.list.Len() >= c.cap {
		delete(c.data, c.list.Remove(c.list.Back()).(*entry).key)
	}
	// 容量足够开始走添加操作
	c.data[key] = c.list.PushFront(&entry{key: key, value: value})
}

func (c *CacheLRU) Set(key, value interface{}) {
	if value == nil {
		c.Delete(key)
		return
	}
	c.Put(key, value)
}

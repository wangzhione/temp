package main

import (
	"container/list"
	"sync"
)

type entry struct {
	key   interface{} // container list 链接 map[interface{}]*list.Element
	value interface{}
}

// CacheLUR
type CacheLRU struct {
	M sync.Mutex // 因为 Get 触发热度更新操作, 所以只能用互斥解决并发冲突

	cap  int                           // capacity 容量 <= 0 标识不限制
	list *list.List                    // 双向链表 entry container list
	data map[interface{}]*list.Element // 数据池子 entry::key -> list::value
}

// NewCacheLRU new CacheLRU return struct pointer
func NewCacheLRU(cap int) *CacheLRU {
	return &CacheLRU{
		cap:  cap,
		list: list.New(),
		data: make(map[interface{}]*list.Element),
	}
}

func (c *CacheLRU) Put(key, value interface{}) {
	element, ok := c.data[key]
	if ok {
		// 结点存在, 更新结点
		element.Value.(*entry).value = value
		// 调整 element 热度
		c.list.MoveToFront(element)
		return
	}

	// 容量不够走删除逻辑
	if c.cap > 0 && c.cap <= c.list.Len() {
		delete(c.data, c.list.Remove(c.list.Back()).(*entry).key)
	}
	// 容量足够开始走添加操作
	c.data[key] = c.list.PushFront(&entry{key: key, value: value})
}

func (c *CacheLRU) Get(key interface{}) (value interface{}, ok bool) {
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
	element, ok := c.data[key]
	if ok {
		delete(c.data, key)
		c.list.Remove(element)
	}
}

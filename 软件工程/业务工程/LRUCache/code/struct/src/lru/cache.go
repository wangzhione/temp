package cache

import (
	"container/list"
	"sync"
)

// entry 存储的实体
type entry struct {
	key, val interface{}
}

// Cache 缓存结构
type Cache struct {
	// m 保证 LRU Cache 访问线程安全
	rw sync.RWMutex

	// max 标识缓存容量的最大值, = 0 标识无限缓存
	max int

	// list 是 entry 循环双向链表
	list *list.List

	// pond entry 缓存池子 key -> entry
	pond map[interface{}]*list.Element
}

// New 构建 LRU Cache 缓存结构
func New(max int) *Cache {
	return &Cache{
		max:  max,
		list: list.New(),
		pond: make(map[interface{}]*list.Element),
	}
}

func (c *Cache) delete(key interface{}) {
	element, ok := c.pond[key]
	if ok {
		delete(c.pond, key)
		c.list.Remove(element)
		return
	}
}

// Set 设置缓存
func (c *Cache) Set(key, val interface{}) {
	c.rw.Lock()
	defer c.rw.Unlock()

	// 看是否进入删除分支
	if val == nil {
		c.delete(key)
		return
	}

	element, ok := c.pond[key]
	if ok {
		// set key nil 进入删除逻辑
		if val == nil {
			delete(c.pond, key)
			c.list.Remove(element)
			return
		}

		// 重新设置 value 数据
		element.Value.(*entry).val = val
		// set key nil exists 进入 update 逻辑
		c.list.MoveToFront(element)
		return
	}

	if val == nil {
		return
	}

	// 首次添加
	c.pond[key] = c.list.PushFront(&entry{key, val})

	// 数据过多, 删除尾巴数据
	if c.list.Len() > c.max && c.max > 0 {
		delete(c.pond, c.list.Remove(c.list.Back()).(*entry).key)
	}
}

// Get 获取缓存
func (c *Cache) Get(key interface{}) (val interface{}, ok bool) {
	c.rw.RLock()
	defer c.rw.RUnlock()

	if element, ok := c.pond[key]; ok {
		// 获取指定缓存值
		val, ok = element.Value.(*entry).val, true
		// 调整缓存热点
		c.list.MoveToFront(element)
	}
	return
}

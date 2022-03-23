package main

import (
	"fmt"
	"testing"
)

func TestCacheLRU_Set(t *testing.T) {
	// 创建
	c := NewCacheLRU(1)

	// 设置
	c.Set("123", "123")
	c.Set("234", "234")

	// 使用
	value, ok := c.Get("123")
	if ok {
		t.Fatal("c.Get 123 fatal, value = ", value)
	}

	fmt.Println(value, ok)
	fmt.Println(c.Get("234"))

	// 删除
	c.Set("234", nil)
	fmt.Println(c.Get("234"))
}

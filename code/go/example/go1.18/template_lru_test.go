package main

import (
	"fmt"
	"testing"
)

func TestNewTemplateLRU(t *testing.T) {
	// 创建
	c := NewTemplateLRU[string, string](1)

	// 设置
	c.Put("123", "123")
	c.Put("234", "234")

	// 使用
	value, ok := c.Get("123")
	if ok {
		t.Fatal("c.Get 123 fatal, value = ", value)
	}

	fmt.Println(value, ok)
	fmt.Println(c.Get("234"))

	// 删除
	c.Delete("234")
	fmt.Println(c.Get("234"))
}

// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Package list implements a doubly linked list.
//
// To iterate over a list (where l is a *List):
//	for e := l.Front(); e != nil; e = e.Next() {
//		// do something with e.Value
//	}
//
package list

import (
	"container/list"
	"fmt"
	"testing"
)

func Test_List_Demo(t *testing.T) {

	// Persion 普通人
	type Persion struct {
		Name string
		Age  int
	}

	pers := list.New()

	// 链表数据填充
	pers.PushBack(&Persion{Name: "wang", Age: 31})
	pers.PushFront(&Persion{Name: "zhi", Age: 31})

	fmt.Printf("List Len() = %d\n", pers.Len())
	if pers.Len() != 2 {
		t.Fatal("pers.Len() != 2 data faild")
	}

	// 开始遍历数据
	for element := pers.Front(); element != nil; element = element.Next() {
		per, ok := element.Value.(*Persion)
		if !ok {
			panic(fmt.Sprint("Persion list faild", element.Value))
		}
		fmt.Println(per)
	}

	// 数据删除
	for element := pers.Front(); element != nil; {
		next := element.Next()
		pers.Remove(element)
		element = next
	}

	fmt.Printf("List Len() = %d\n", pers.Len())
	if pers.Len() != 0 {
		t.Fatal("pers.Len() != 0 data faild")
	}
}

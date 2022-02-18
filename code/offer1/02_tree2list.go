package main

import (
	"container/list"
	"fmt"
)

type Tree struct {
	Left  *Tree
	Right *Tree

	Value int
}

func (t *Tree) print() {
	fmt.Printf(" %d ", t.Value)
}

func treeCreate() *Tree {
	// 1 2 3 4 5 6 7 8 9
	/*
				5
			   /  \
			  3    7
			 / \   /\
			2   4 6  8
		   /          \
		  1            9
	*/

	var nodes [9]*Tree
	for i := 0; i < 9; i++ {
		nodes[i] = &Tree{Value: i + 1}
	}

	nodes[4].Left = nodes[2]
	nodes[4].Right = nodes[6]

	nodes[2].Left = nodes[1]
	nodes[2].Right = nodes[3]

	nodes[1].Left = nodes[0]

	nodes[6].Left = nodes[5]
	nodes[6].Right = nodes[7]

	nodes[7].Right = nodes[8]

	return nodes[4]
}

func treeInorder(root *Tree) {
	if root != nil {
		// 左 -> 中 -> 右
		treeInorder(root.Left)
		root.print()
		treeInorder(root.Right)
	}
}

func main() {
	root := treeCreate()

	treeInorder(root)
	fmt.Println()

	treeToList(root)
	fmt.Println()
}

func treeToList(root *Tree) {
	if root == nil {
		return
	}

	stack := list.New()
	treeToListRetRoot(root, stack)
}

func treeToListRetRoot(root *Tree, stack *list.List) {
	for stack.Len() > 0 || root != nil {
		for root != nil {
			stack.PushBack(root)
			root = root.Left
		}

		back := stack.Back()
		root = back.Value.(*Tree)
		stack.Remove(back)

		root.print()
		root = root.Right
	}
}

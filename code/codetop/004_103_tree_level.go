package main

import (
	"encoding/json"
	"fmt"
	"os"
)

/* describe : 103. 二叉树的锯齿形状层序遍历

   给你二叉树根结点 root, 返回其结点值的锯齿形层序遍历.
   (即先从左往右, 再从右往左进行下一层遍历, 以此类推, 层与层之间交替进行).
*/

type TreeNode struct {
	Val   int
	Left  *TreeNode
	Right *TreeNode
}

func zigzagLevelOrder(root *TreeNode) [][]int {
	var res [][]int

	if root == nil {
		return res
	}
	if root.Left == nil && root.Right == nil {
		return [][]int{{root.Val}}
	}

	// true 正向, false 反向
	direction := false
	// 存储数据集合
	data := []*TreeNode{root}

	for len(data) > 0 {
		var next []*TreeNode
		for _, value := range data {
			if value.Left != nil {
				next = append(next, value.Left)
			}
			if value.Right != nil {
				next = append(next, value.Right)
			}
		}

		// 存储 data 数据.
		var now []int
		direction = !direction
		if direction {
			for i := 0; i < len(data); i++ {
				now = append(now, data[i].Val)
			}
		} else {
			for i := len(data) - 1; i >= 0; i-- {
				now = append(now, data[i].Val)
			}
		}
		res = append(res, now)

		// 继续下一轮
		data = next
	}

	return res
}

// build:
// go run 004_103_tree_level.go
func main() {
	root := TreeNode{
		Val:  3,
		Left: &TreeNode{Val: 9},
		Right: &TreeNode{
			Val:   20,
			Left:  &TreeNode{Val: 15},
			Right: &TreeNode{Val: 7},
		},
	}

	fmt.Printf("root = %v\n", root)

	data, err := json.Marshal(root)
	if err != nil {
		fmt.Printf("json.Marshal error = %+v\n", err)
		os.Exit(-1)
	}
	fmt.Printf("root = %s\n", data)

	res := zigzagLevelOrder(&root)
	fmt.Printf("res = %+v\n", res)
}

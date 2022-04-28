package main

/* describe : 199. 二叉树的右视图

   给定一个二叉树根结点 root, 想象自己站在他的右侧, 按照从顶部到底部的顺序,
   返回从右侧所能看到的结点值.

   示例1:
        1
       / \
      2   3
       \   \
        5   4
    输入: [1, 2, 3, NULL, 5, NULL, 4]
    输出: [1, 3, 4]

   示例2:
   输入: [1, NULL, 3]
   输出: [1, 3]

   示例3:
   输入: []
   输出: []
*/

// v Definition for a binary tree node
type TreeNode struct {
	Val   int
	Left  *TreeNode
	Right *TreeNode
}

func rightSideView(root *TreeNode) (res []int) {
	if root == nil {
		return
	}

	queue1 := []*TreeNode{root}
	queue2 := []*TreeNode{}
	for {
		// 存储右视图结果
		res = append(res, queue1[len(queue1)-1].Val)

		queue2 = queue1[0:0]
		for i := 0; i < len(queue1); i++ {
			if queue1[i].Left != nil {
				queue2 = append(queue2, queue1[i].Left)
			}
			if queue1[i].Right != nil {
				queue2 = append(queue2, queue1[i].Right)
			}
		}
		queue1 = queue2

		if len(queue1) <= 0 {
			break
		}
	}

	return
}

// build:
// go run 013_199_tree_right_side_view.go
//
func main() {

}

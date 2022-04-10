package main

import (
	"fmt"
	"sort"
)

// 15. 三数之和
// 给你一个包含 n 个整数的数组 nums, 判断 nums 中是否存在三个元素
// a, b, c 使得 a + b + c = 0 ? 请你找出所有和为 0 且不重复的三元组.
// 注意: 答案中不可以包含重复的三元组

func threeSum(nums []int) [][]int {
	numsLen := len(nums)

	// 特殊 case 处理
	if numsLen < 3 {
		// 找不到 a, b, c 直接返回
		return nil
	}
	if numsLen == 3 {
		if nums[0]+nums[1]+nums[2] == 0 {
			return [][]int{nums}
		}
		return nil
	}

	// 开始排序
	sort.Ints(nums)

	var res [][]int
	// 开始将 a + b + c = 0 转换为 sum = -a = b + c
	for i := 0; i < numsLen-3; i++ {
		a := nums[i]
		if a > 0 {
			break
		}

		// 开始遍历
		for left, right := i+1, numsLen-1; left < right; {
			b, c := nums[left], nums[right]
			sum := a + b + c
			if sum == 0 {
				res = append(res, []int{a, nums[left], nums[right]})
				left++
				for left < right && nums[left] == b {
					left++
				}
				right--
				for left < right && nums[right] == c {
					right--
				}
			} else if sum < 0 {
				left++
			} else {
				right--
			}
		}

		// 跳跃
		for i < numsLen-3 && nums[i+1] == a {
			i++
		}
	}

	return res
}

type snum []int

func (nums snum) Len() int {
	return len(nums)
}

func (nums snum) Less(i, j int) bool {
	return nums[i] <= nums[j]
}

func (nums snum) Swap(i, j int) {
	nums[i], nums[j] = nums[j], nums[i]
}

// build :
// go run 005_15_array_three_sum.go
func main() {
	nums := []int{-1, 0, 1, 2, -1, -4}
	fmt.Printf("nums = %+v\n", nums)

	sort.Sort(snum(nums))
	fmt.Printf("nums = %+v\n", nums)

	res := threeSum(nums)
	fmt.Printf("res = %+v\n", res)
}

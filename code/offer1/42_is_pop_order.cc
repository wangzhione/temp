#include <iostream>
#include <stack>

/* describe:
   题目输入两个整数序列, 第一个序列表示栈的压入顺序, 请判断第二个序列是否为该栈的弹出顺序.
   假设压入栈的所有数字均不相等. 例如序列 1, 2, 3, 4, 5 是某栈的压栈序列, 序列 4, 5, 3, 2, 1 
   是该压栈序列对应的一个弹出序列, 但 4, 3, 5, 1, 2 就不可能是该压栈序列的弹出序列.

   answer:
   总结上述入栈, 出栈的过程, 我们可以找到判断一个序列是不是栈的弹出序列规律:
   如果下一个弹出的数字刚好是栈顶数字, 那么直接弹出. 如果下一个弹出的数字不在栈顶,
   我们把压栈序列中还没有入栈的数字压入辅助栈, 直到把下一个弹出的数字压入栈顶为止.
   如果所有的数字都压入栈了仍然没有找到下一个弹出的数字, 那么该序列不可能是一个弹出序列.
 */

bool IsPopOrder(const int * pPush, const int * pPop, int nLength) {
    if (pPush == nullptr || pPop == nullptr || nLength <= 0) {
        return false;
    }

    const int * pNextPush = pPush;
    const int * pNextPop = pPop;

    std::stack<int> stackData;

    while (pNextPop - pPop < nLength) {
        while (stackData.empty() || stackData.top() != *pNextPop) {
            if (pNextPush - pPush >= nLength)
                break;
            
            stackData.push(*pNextPush);
            pNextPush++;
        }

        if (stackData.top() != *pNextPop)
            break;

        stackData.pop();
        pNextPop++;
    }

    return stackData.empty() && pNextPop - pPop == nLength;
}

// build:
// g++ -g -O3 -Wall -Wextra -Werror -o 42_is_pop_order 42_is_pop_order.cc
//
int main(void) {

    exit(EXIT_SUCCESS);
}
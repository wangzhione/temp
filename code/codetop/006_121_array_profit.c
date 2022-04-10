#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 121. 买卖股票的最佳时机

   给定一个数组 prices, 他的第 i 个元素 prices[i] 表示一支给定股票第 i 天的价格.
   你只能选择 某一天 买入这只股票, 并选择在 未来的某一个不同的日子 卖出该股票. 设计一个算法来计算你所能获取的最大利润.
   返回你可以从这笔交易中获取的最大利润. 如果你不能获取任何利润, 返回 0.
 */

int maxProfit0(int * prices, int pricesSize) {
    // 没有数据或者只有一天数据, 直接返回
    if (prices == NULL || pricesSize <= 1) {
        return 0;
    }

    int max = 0;
    for (int i = 0; i < pricesSize-1; i++) {
        for (int j = i+1; j < pricesSize; j++) {
            if (max < prices[j] - prices[i]) {
                max = prices[j] - prices[i];
            }
        }
    }


    return max;
}

int maxProfit2(int * prices, int pricesSize) {
    // 没有数据或者只有一天数据, 直接返回
    if (prices == NULL || pricesSize <= 1) {
        return 0;
    }
    if (pricesSize == 2) {
        return prices[0] < prices[1] ? prices[1] - prices[0] : 0;
    }

    int max = 0;
    int min = 0;
    int i = 1;
    while (i < pricesSize) {
        if (prices[max] < prices[i]) {
            max = i;
        }
        if (prices[min] > prices[i]) {
            min = i;
        }
        i++;
    }
    if (min <= max) {
        return prices[max] - prices[min];
    }

    // [begin, max, min, end] -> [begin, max], (max, min), [min, end]

    // 开始分两步查, min 右边找最大. max 左边找最小
    int minmax = 0;
    for (i = min+1; i < pricesSize; i++) {
        if (minmax < prices[i]-prices[min]) {
            minmax = prices[i]-prices[min];
        }
    }

    int maxmin = 0;
    for (i = 0; i < max; i++)  {
        if (maxmin < prices[max]-prices[i]) {
            maxmin = prices[max]-prices[i];
        }
    }

    minmax = minmax < maxmin ? maxmin : minmax;
    if (max+1 < min-1) {
        int middlen = maxProfit2(prices+max+1, min-1-max);
        if (minmax < middlen) {
            minmax = middlen;
        }
    }
    return minmax;
}

int maxProfit3(int * prices, int pricesSize) {
    if (prices == NULL || pricesSize <= 1) {
        return 0;
    }

    int minPrice = prices[0];
    // max prifit 最大利润
    int max = 0;

    for (int * ptr = prices+1, * end = prices+pricesSize; ptr < end; ptr++) {
        int price = *ptr;
        if (minPrice > price) {
            minPrice = price;
        } else {
            int diff = price - minPrice;
            if (diff > max) {
                max = diff;
            }
        }
    }

    return max;
}

int maxProfit(int * prices, int pricesSize) {
    if (prices == NULL || pricesSize <= 1) {
        return 0;
    }

    int minPrice = prices[0];
    // max prifit 最大利润
    int max = 0;

    for (int i = 1; i < pricesSize; i++) {
        if (prices[i] < minPrice) {
            minPrice = prices[i];
        } else if (prices[i] - minPrice > max) {
            max = prices[i] - minPrice;
        }
    }

    return max;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 006_121_array_profit 006_121_array_profit.c
//
int main(void) {
    int prices[] = {7, 2, 4, 1};
    int pricesSize = sizeof prices / sizeof (int);

    int max = maxProfit(prices, pricesSize);
    printf("max = %d\n", max);

    exit(EXIT_SUCCESS);
}

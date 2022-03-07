#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 

   从扑克牌中随机抽 5 张牌, 判断是不是一个顺子, 即这 5 张牌是不是连续的.
   2 ~ 10 为数字本身, A 为 1, J 为 11, Q 为 12, K 为 13, 而大, 小王可以看成任意数字.
 */

/*
    poker/playing card 扑克牌；
    ace 尖儿；
    big joker 大王；
    little joker 小王；
    club 梅花；
    diamond 方块；
    heart 红桃；
    face cards/court cards 花牌（J、Q、K）；
    spade 黑桃；
    straight flush 同花顺；
    shuffle 洗牌；
    pass 不要；
    deal 分牌
 */

#define CARD_COLOR_CLUB         1
#define CARD_COLOR_DIAMOND      2
#define CARD_COLOR_HEART        3
#define CARD_COLOR_SPADE        4
#define CARD_COLOR_BIG_JOKER    5
#define CARD_COLOR_LITTLE_JOKER 6

/*
 value  : 
 A      : 1
 2      : 2
 3      : 3
 4      : 4
 5      : 5
 6      : 6
 7      : 7
 8      : 8
 9      : 9
 10     : 10
 J      : 11
 Q      : 12
 K      : 13
 */

struct card {
    uint8_t color;      // 颜色
    uint8_t value;      // 值
};

static void card_value(struct card * d, char * cards) {
    switch (d->value) {
    case 0:
        return;
    case 1:
        cards[0] = 'A';
        return;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        cards[0] = d->value + '0';
        return;
    case 10:
        cards[0] = '1';
        cards[1] = '0';
        return;
    case 11:
        cards[0] = 'J';
        return;
    case 12:
        cards[0] = 'Q';
        return;
    case 13:
        cards[0] = 'K';
        return;
    default:
        fprintf(stderr, "{color:%d, value:%d} error\n", d->color, d->value);
        return;
    }
}

// ♣ □ ♥ ♠ 4 字节, 10 2 字节, 0 结尾
// https://zh.wikipedia.org/wiki/%E6%92%B2%E5%85%8B%E7%89%8C_(Unicode%E5%8D%80%E6%AE%B5)
// https://www.compart.com/en/unicode/U+1F0DF
// char cards[4+2+1] = {0};
// 更好方式通过, UTF-8 编码处理
#define CARD_DRAW_INT (4+2+1)

void card_draw(struct card * d, char tmp[static CARD_DRAW_INT]) {
    memset(tmp, 0, CARD_DRAW_INT);

    assert(d != NULL && d->value >= 0 && d->value <= 13);
    switch (d->color) {
    case CARD_COLOR_CLUB:
        // 也可以用定点编码, 希望也可以自己弄
        memcpy(tmp, "♣", sizeof "♣");
        card_value(d, tmp+sizeof "♣"-1);
        break;
    case CARD_COLOR_DIAMOND:
        memcpy(tmp, "□", sizeof "□");
        card_value(d, tmp+sizeof "□"-1);
        break;
    case CARD_COLOR_HEART:
        memcpy(tmp, "♥", sizeof "♥");
        card_value(d, tmp+sizeof "♥"-1);
        break;
    case CARD_COLOR_SPADE:
        memcpy(tmp, "♠", sizeof "♠");
        card_value(d, tmp+sizeof "♠"-1);
        break;
    case CARD_COLOR_BIG_JOKER:
        // 🃏 0xF0 0x9F 0x83 0x8F
        tmp[0] = 0xF0;
        tmp[1] = 0x9F;
        tmp[2] = 0x83;
        tmp[3] = 0x8F;
        break;
    case CARD_COLOR_LITTLE_JOKER:
        // 🃟 0xF0 0x9F 0x83 0x9F
        tmp[0] = 0xF0;
        tmp[1] = 0x9F;
        tmp[2] = 0x83;
        tmp[3] = 0x9F;
        break;
    default:
        fprintf(stderr, "{color:%d, value:%d} error\n", d->color, d->value);
        return;
    }
}

#define CARDS_DATA_LEN  (4*13 + 2)

struct cards {
    struct card data[CARDS_DATA_LEN];
};

struct cards * cards_create(void) {
    // 54 = 4*13 + 2;
    struct cards * cards = calloc(1, sizeof(struct cards));

    // 开始初始化, 先初始化 4*13
    for (int i = CARD_COLOR_CLUB; i <= CARD_COLOR_SPADE; i++) {
        for (int j = 1; j <= 13; j++) {
            struct card * card = &cards->data[(i-1)*13 + j - 1];
            card->color = i;
            card->value = j;
        }
    }

    // 再初始化大小王
    cards->data[CARDS_DATA_LEN-2].color = CARD_COLOR_LITTLE_JOKER;
    cards->data[CARDS_DATA_LEN-1].color = CARD_COLOR_BIG_JOKER;

    // 开始洗牌
    for (int i = 0; i < CARDS_DATA_LEN; i++) {
        int left = rand() % CARDS_DATA_LEN;
        int right = rand() % CARDS_DATA_LEN;
        if (left != right) {
            struct card tmp = cards->data[left];
            cards->data[left] = cards->data[right];
            cards->data[right] = tmp;
        }
    }

    return cards;
}

void cards_draw(struct cards * cards) {
    int i, j;
    char tmp[CARD_DRAW_INT];
    struct card * card;

    for (i = CARD_COLOR_CLUB; i <= CARD_COLOR_SPADE; i++) {
        for (j = 1; j <= 13; j++) {
            card = &cards->data[(i-1)*13 + j - 1];
            card_draw(card, tmp);
            printf("%*s", CARD_DRAW_INT, tmp);
        }
        printf("\n");
    }

    // 输出大小王
    card = &cards->data[CARDS_DATA_LEN-2];
    card_draw(card, tmp);
    printf("%*s", CARD_DRAW_INT, tmp);

    card = &cards->data[CARDS_DATA_LEN-1];
    card_draw(card, tmp);
    printf("%*s", CARD_DRAW_INT, tmp);
    printf("\n");
}

bool cards_order(struct card data[static 5]) {
    struct card temp[5];
    memcpy(temp, data, sizeof(struct card)*5);

    int i;
    // 对 temp 进行排序
    for (i = 1; i < 5; i++) {
        struct card tc = temp[i];
        int j = i - 1;
        while (j >= 0 && tc.value < temp[j].value) {
            temp[j+1] = temp[j];
            j--;
        }
        temp[j+1] = tc;
    }

    // debug printf
    printf("cards_order debug printf:\n");
    char tmp[CARD_DRAW_INT];
    for (i = 0; i < 5; i++) {
        card_draw(temp+i, tmp);
        printf("%*s", CARD_DRAW_INT, tmp);
    }
    printf("\n");

    // 开始判断处理
    int joker = 0; // 大小王数量
    for (i = 0; i < 5; i++) {
        int color = temp[i].color;
        // temp[i].value == 0
        // temp[i].color == CARD_COLOR_BIG_JOKER || temp[i].color == CARD_COLOR_LITTLE_JOKER
        if (color == CARD_COLOR_BIG_JOKER || color == CARD_COLOR_LITTLE_JOKER) {
            joker++;
            continue;
        }
        break;
    }
    printf("debug 1 i= %d, joker = %d\n", i, joker);
    // 开始判断
    while (++i < 5) {
        int diff = temp[i].value - temp[i-1].value;
        assert(diff >= 0);
        // 这种相似的牌一定不是顺子
        if (diff == 0) {
            return false;
        }
        // 正常顺子
        if (diff == 1) {
            continue;
        }
        // 距离超过了大小王能弥补的也直接返回
        if (diff-1 > joker) {
            return false;
        }
        joker -= diff-1;
    }
    printf("debug 2 i= %d, joker = %d\n", i, joker);
    return joker >= 0;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 66_card 66_card.c
//
int main(void) {
    char str[5] = "🃏";
    printf("str = %s\n", str);
    str[0] = 0xF0;
    str[1] = 0x9F;
    str[2] = 0x83;
    str[3] = 0x9F;
    printf("str = %s\n", str);

    // wpritnf 和 printf 不能混用
    // wchar_t wc = 0x0001F0DE;
    // wprintf(L"wc = %c, %s\n", wc, str);
    printf("str = %s\n", str);

    printf("sizeof card = %zu, sizeof cards = %zu\n", sizeof(struct card), sizeof(struct cards));

    // 开始渲染
    // srand((unsigned)time(NULL));
    struct cards * cards = cards_create();
    cards_draw(cards);

    bool status = cards_order(cards->data);
    printf("status = %d\n", status);

    struct card data[5] = {0};
    data[0].color = CARD_COLOR_BIG_JOKER;
    data[1].color = CARD_COLOR_LITTLE_JOKER;
    data[2].color = CARD_COLOR_CLUB;
    data[2].value = 13;
    data[3].color = CARD_COLOR_DIAMOND;
    data[3].value = 8;
    data[4].color = CARD_COLOR_HEART;
    data[4].value = 9;

    status = cards_order(data);
    printf("status = %d\n", status);

    exit(EXIT_SUCCESS);
}

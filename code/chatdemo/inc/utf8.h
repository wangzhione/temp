// UTF-8 工具函数, 负责校验字节序列并统计 Unicode 字符数
#pragma once

#include "platform.h"

// 校验 UTF-8 文本并统计字符数; 成功返回 0, 失败返回 -EINVAL 或 -EILSEQ
extern int utf8_validate_and_count(const char * text, size_t text_len, size_t * char_count);


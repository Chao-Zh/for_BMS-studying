#pragma once

#include "model.h"

int saveCard(const Card *pCard, const char *pPath);               // 将卡信息保存到文件中
int readCard(Card *pCard, const char *pPath);                     // 从文件中读取卡信息到结构体数组
int getCardCount(const char *pPath);                              // 获取卡信息文件中，卡信息数量
int updateCard(const Card *pCard, const char *pPath, int nIndex); // 更新卡信息文件中的一条卡信息
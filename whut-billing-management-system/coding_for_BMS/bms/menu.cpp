#include <iostream>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <cstring>
#include <math.h>

#include "model.h"
#include "menu.h"
#include "card_service.h"
#include "global.h"
#include "tool.h"
#include "service.h"
#include "billing_service.h"
#include "billing_file.h"

using namespace std;

char fmt[64];//储存生成的格式字符串，也防止溢出作为缓冲区


// 函数名:outputMenu
// 功能:输出系统菜单
// 参数:void
// 返回值:void
void outputMenu()
{ // 输出系统菜单
	printf("\n");
	printf("------计费系统菜单------\n");
	printf("*                      *\n");
	printf("*       1.添加卡       *\n");
	printf("*       2.查询卡       *\n");
	printf("*       3.上机         *\n");
	printf("*       4.下机         *\n");
	printf("*       5.充值         *\n");
	printf("*       6.退费         *\n");
	printf("*       7.查询统计     *\n");
	printf("*       8.注销卡       *\n");
	printf("*       0.退出         *\n");
	printf("------------------------\n\n");
}

// 函数名:add
// 功能:添加用户卡信息到卡结构体变量，并屏幕显示
// 参数:void
// 返回值:void
void add()
{
    Card card = {}; // 零初始化
    char name[30];
    char pwd[20];

    // 输入卡号
    printf("请输入卡号(长度1~18):");
    scanf("%18s", name);

	// 中文检测逻辑
    for (int i = 0; name[i] != '\0'; i++) {
        // 检测最高位是否为1（ASCII字符最高位为0）
        if ((unsigned char)name[i] >= 0x80) {
            printf("卡号中不能包含中文字符以及非法字符！\n");
            return;
        }
    }
    if (strlen(name) >= 18) {
        printf("卡号过长！\n");
        return;
    }
    if (queryCardInfo(name)) {
        printf("卡号已存在！\n");
        return;
    }
    strncpy(card.aName, name, sizeof(card.aName)-1);

    // 输入密码
    printf("请输入密码(长度1~8):");
    scanf("%8s", pwd);
    strncpy(card.aPwd, pwd, sizeof(card.aPwd)-1);

	// 中文检测逻辑
    for (int i = 0; pwd[i] != '\0'; i++) {
        // 检测最高位是否为1（ASCII字符最高位为0）
        if ((unsigned char)pwd[i] >= 0x80) {
            printf("密码不能包含中文字符以及非法字符！\n");
            return;
        }
    }

	strncpy(card.aPwd, pwd, sizeof(card.aPwd)-1);

    // 输入金额
    printf("请输入开卡金额(RMB):");
    scanf("%f", &card.fBalance);

	
// 初始化时间字段
card.tStart = time(NULL);  // 开卡时间设为当前时间
card.tLastTime = card.tStart;

// 计算截止时间（当前时间 + 1年）
struct tm tm_start;
struct tm tm_end;

// 将当前时间转换为本地时间结构体
if (!localtime_r(&card.tStart, &tm_start)) {
    perror("时间转换失败");
    return;
}

// 复制时间结构体并增加1年
tm_end = tm_start;
tm_end.tm_year += 1;  // tm_year 是自1900的年数

// 自动修正非法日期（如2月29日加1年后变为3月1日）
card.tEnd = mktime(&tm_end);
if (card.tEnd == -1) {
    perror("无效的截止时间");
    return;
}

// 验证截止时间
struct tm tm_check;
if (!localtime_r(&card.tEnd, &tm_check)) {
    perror("截止时间转换失败");
    return;
}

    // 输出信息
    printf("----------------添加的卡信息如下----------------\n");
	getLeftAlignFormat(fmt, CARD_NO_WIDTH, "卡号");
	printf(fmt, "卡号"); 
	getLeftAlignFormat(fmt, PWD_WIDTH, "密码");
	printf(fmt, "密码");
	getLeftAlignFormat(fmt, STATUS_WIDTH, "状态");
	printf(fmt, "状态");
	getLeftAlignFormat(fmt, INIT_BALANCE_WIDTH, "开卡金额");
	printf(fmt, "开卡金额");
	printf("\n");
    // 输出所添加卡信息
	printf("%-*s%-*s%-*d%-*.2f\n",
    CARD_NO_WIDTH, card.aName,
    PWD_WIDTH, card.aPwd,
    STATUS_WIDTH, card.nStatus,
    INIT_BALANCE_WIDTH, card.fBalance);
    printf("--------------------------------------------------\n");

    // 保存到文件
    if (!addCardInfo(card)) {
        printf("添加失败！\n");
    } else {
        printf("添加成功！\n");
    }
}

// 函数名:getSize
// 功能:计算字符数组中字符长度
// 参数:字符数组名
// 返回值:字符个数
int getSize(const char *pString)
{
	int nSize = 0;
	// 计算字符串的字符个数
	while (*(pString + nSize) != '\0')
	{
		nSize++;
	}
	// 返回字符个数
	return nSize;
}

// 函数名:query
// 功能:根据输入的卡号调用，查询是否有该卡，有的话，输出卡信息
// 参数:void
// 返回值:void
void query()
{
	char name[18]; // 存放要查询的用户名
	Card *pCard = NULL;
	char aLastTime[30]; // 存放指定格式字符串的时间
	int icha = 0;
	int nIndex = 0;
	int i; // 查询到卡信息数量

	printf("请输入要查询的卡号(长度1~18):");
	scanf("%18s", name);

	printf("1.精确查询||2.模糊查询(输入1或2):");
	scanf("%d", &icha);

	if (icha == 1)
	{
		pCard = queryCardInfo(name);
	}
	else if (icha == 2)
	{
		pCard = queryCardsInfo(name, &nIndex);
	}
	else
	{
		do
		{
			printf("输入错误！\n请重新输入:");
			scanf("%d", &icha);
		} while (icha != 1 && icha != 2);
		if (icha == 1)
		{
			pCard = queryCard(name);
		}
		else if (icha == 2)
		{
			pCard = queryCards(name, &nIndex);
		}
	}

	if (pCard == NULL)
	{
		printf("----*****-----没有该卡的信息！-----*****----\n");
	}
	else
	{
		if (icha == 1)
		{
		if (pCard->nStatus == 2) {  // 注意这里用指针运算符->
                printf("该卡已经注销！\n");
                return;  // 直接返回不显示详细信息
			}
		
			printf("-------------******-------------查询到的卡信息如下-------------******-------------\n");
			// 输出表格的表头
			getLeftAlignFormat(fmt, CARD_NO_WIDTH, "卡号");
			printf(fmt, "卡号"); 
			getLeftAlignFormat(fmt, STATUS_WIDTH, "状态");
			printf(fmt, "状态");
			getLeftAlignFormat(fmt, BALANCE_WIDTH, "余额");
			printf(fmt, "余额");
			getLeftAlignFormat(fmt, COL_TOTAL_USE, "累计金额");
			printf(fmt, "累计金额");
			getLeftAlignFormat(fmt, COL_USE_COUNT, "使用次数");
			printf(fmt, "使用次数");
			getLeftAlignFormat(fmt, TIME_WIDTH, "上次使用时间");
			printf(fmt, "上次使用时间");
			printf("\n");

			// 将time_t类型时间转换为字符串，字符串格式为“年-月-日 时：分”
			timeToString(pCard->tLastTime, aLastTime);
			// 输出查到的卡信息
			printf("%-*s%-*d%-*.2f%-*.2f%-*d%-*s\n",
        	CARD_NO_WIDTH, pCard->aName,
        	STATUS_WIDTH, pCard->nStatus,
        	BALANCE_WIDTH, pCard->fBalance,
        	COL_TOTAL_USE, pCard->fTotalUse,
        	COL_USE_COUNT, pCard->nUseCount,
        	TIME_WIDTH, aLastTime);
			printf("----------------------------------------------------------------------------------\n");
		}
		else // 模糊查询结果输出
		{
			printf("-------------******-------------查询到的卡信息如下-------------******-------------\n");
			// 输出表格的表头
			getLeftAlignFormat(fmt, CARD_NO_WIDTH, "卡号");
			printf(fmt, "卡号"); 
			getLeftAlignFormat(fmt, STATUS_WIDTH, "状态");
			printf(fmt, "状态");
			getLeftAlignFormat(fmt, BALANCE_WIDTH, "余额");
			printf(fmt, "余额");
			getLeftAlignFormat(fmt, COL_TOTAL_USE, "累计金额");
			printf(fmt, "累计金额");
			getLeftAlignFormat(fmt, COL_USE_COUNT, "使用次数");
			printf(fmt, "使用次数");
			getLeftAlignFormat(fmt, TIME_WIDTH, "上次使用时间");
			printf(fmt, "上次使用时间");
			printf("\n");

			for (i = 0; i < nIndex; i++)
			{
				// 将time_t类型时间转化为字符串，字符串格式为"年-月-日 时：分"
				timeToString(pCard[i].tLastTime, aLastTime); // 结构体指针当数组名用

				// 输出查询到的卡信息
				printf("%-*s%-*d%-*.2f%-*.2f%-*d%-*s\n",
        		CARD_NO_WIDTH, pCard[i].aName,
        		STATUS_WIDTH, pCard[i].nStatus,
        		BALANCE_WIDTH, pCard[i].fBalance,
        		COL_TOTAL_USE, pCard[i].fTotalUse,
        		COL_USE_COUNT, pCard[i].nUseCount,
        		TIME_WIDTH, aLastTime);
				printf("----------------------------------------------------------------------------------\n");
			}

			// 释放动态分配的内存
			free(pCard);
		}
		pCard = NULL;
	}
}

// 函数名:exitApp
// 功能:退出应用程序
// 参数:void
// 返回值:void
void exitApp()
{
	releaseList();
}

// 函数名:logon
// 功能:输入上机卡信息，查询到，输出上机卡信息
// 参数:void
// 返回值:void
void logon()
{
	char aName[30] = {0}; // 上机卡号
	char aPwd[20] = {0};  // 上机密码
	char aLastTime[30];	  // 存放指定格式字符串的时间
	LogonIofo *pInfo = NULL;
	int nResult = 0;

	// 提示并接受上机卡号
	printf("请输入上机的卡号(长度为0~18):");
	scanf("%30s", aName);
	// 判断输入的卡号是否符合要求
	if (getSize(aName) >= 18)
	{
		printf("输入的卡号长度超过最大值！\n");
		return;
	}
	// 提示并接受上机密码
	printf("请输入上机密码(长度为1~8):");
	scanf("%20s", aPwd);
	// 判断输入的密码是否符合要求
	if (getSize(aPwd) >= 8)
	{
		printf("输入的密码长度超过最大值！\n");
		return;
	}
	// 开始上机，获取上机结果
	pInfo = (LogonInfo *)malloc(sizeof(LogonInfo));
	if (pInfo == NULL)
	{
		exit(1);
	}
	// 根据上机结果，提示不同信息
	nResult = doLogon(aName, aPwd, pInfo);

	switch (nResult)
	{
	case 0:
		printf("上机失败!\n");
		break;
	case 1:
		printf("------***------上机信息如下------***------\n");
		// 输出表格的表头
		getLeftAlignFormat(fmt, CARD_NO_WIDTH, "卡号");
		printf(fmt, "卡号"); 
		getLeftAlignFormat(fmt, BALANCE_WIDTH, "余额");
		printf(fmt, "余额");
		getLeftAlignFormat(fmt, TIME_WIDTH, "上机时间");
		printf(fmt, "上机时间");
		printf("\n");

		// 将time_t类型时间转化为字符串，字符串格式为“年-月-日 时：分”
		timeToString(pInfo->tLogon, aLastTime); // 结构指针当数组名使用
		// 输出上机卡信息
		printf("%-*s%-*.2f%-*s\n",
    	CARD_NO_WIDTH, pInfo->aCardName,
    	BALANCE_WIDTH, pInfo->fBalance,
    	TIME_WIDTH, aLastTime);
		

		printf("------------------------------------------\n");
		printf("---------上机成功！---------\n");
		break;
	case 2:
		printf("---------该卡不能使用!---------\n");
		break;
	case 3:
		printf("---------余额不足！---------\n");
		break;
	}
	// 释放上机信息
	free(pInfo);
}

// 函数名:settle
// 功能:输入下机卡信息，查询到输出上机卡信息
// 参数:void
// 返回值:void
void settle()
{
	char aName[18] = {0};	   // 卡号
	char aPwd[8] = {0};		   // 密码
	int nResult = -1;		   // 下机结果
	SettleInfo *pInfo = NULL;  // 下机卡信息
	char aStartTime[30] = {0}; // 上机时间
	char aEndTime[30] = {0};   // 下机时间

	// 为下机信息动态分配内存
	pInfo = (SettleInfo *)malloc(sizeof(SettleInfo));
	if (pInfo == 0)
	{
		exit(1);
	}
	printf("请输入下机卡号(长度1~18):");
	scanf("%18s", aName);
	// 判断输入的卡号是否符合要求
	if (getSize(aName) >= 18)
	{
		printf("输入的卡号长度超过最大值！\n");
		return;
	}

	printf("请输入下机密码(长度1~8):");
	scanf("%8s", aPwd);
	// 判断输入的密码是否符合要求
	if (getSize(aPwd) >= 8)
	{
		printf("输入的密码长度超过最大值！\n");
		return;
	}

	// 进行下机
	nResult = doSettle(aName, aPwd, pInfo);

	// 根据下机结果，提示不同信息
	printf("\n");
	printf("--------------*****--------------下机信息如下--------------*****--------------\n");
	switch (nResult)
	{
	case 0: // 下机失败
	{
		printf("下机失败！\n");
		break;
	}
	case 1: // 下机成功
	{
		// 输出表格表头
		getLeftAlignFormat(fmt, CARD_NO_WIDTH, "卡号");
		printf(fmt, "卡号"); 
		getLeftAlignFormat(fmt, CONSUME_WIDTH, "消费");
		printf(fmt, "消费");
		getLeftAlignFormat(fmt, BALANCE_WIDTH, "余额");
		printf(fmt, "余额");
		getLeftAlignFormat(fmt, TIME_WIDTH, "上机时间");
		printf(fmt, "上机时间");
		getLeftAlignFormat(fmt, TIME_WIDTH, "下机时间");
		printf(fmt, "下机时间");
		printf("\n");
		// 将上机时间默认为卡的最后使用时间，后面添加计费信息后，使用计费信息时间
		// 将time_t类型时间转化为字符串，字符串格式为“年-月-日 时：分”
		timeToString(pInfo->tStart, aStartTime);
		timeToString(pInfo->tEnd, aEndTime);

		// 输出下机卡信息
		printf("%-*s%-*.2f%-*.2f%-*s%-*s\n",
    	CARD_NO_WIDTH, pInfo->aCardName,
    	CONSUME_WIDTH, pInfo->fAmount,
    	BALANCE_WIDTH, pInfo->fBalance,
    	TIME_WIDTH, aStartTime,
    	TIME_WIDTH, aEndTime);
		printf("---------------------------------------------------------------------------\n");
		printf("下机成功！\n");
		break;
	}
	case 2: // 正在使用或已注销
	{
		printf("该卡未在使用！\n");
		break;
	}
	case 3: // 卡余额不足
	{
		printf("卡余额不足！\n");
		break;
	}
	default:
	{
		break;
	}
	}
	// 释放动态分配内存
	free(pInfo);
}

// 函数名:addMoney
// 功能:充值
// 参数:void
// 返回值:void
void addMoney()
{
	char aName[30] = {0}; // 卡号
	char aPwd[20] = {0};	  // 密码
	float fAmount = 0;	  // 充值金额
	MoneyInfo sMoneyInfo; // 充值卡信息

	printf("请输入充值的卡号(长度1~18):");
	scanf("%18s", aName);
	// 判断输入的卡号是否符合要求
	if (getSize(aName) >= 18)
	{
		printf("输入的卡号长度超过最大值！\n");
		return;
	}

	printf("请输入充值密码(长度1~8):");
	scanf("%8s", aPwd);
	// 判断输入的密码是否符合要求
	if (getSize(aPwd) >= 8)
	{
		printf("输入的密码长度超过最大值！\n");
		return;
	}

	cout << "请输入充值金额(RMB):";
	cin >> fAmount;
	cin.clear();
	cin.sync();

	// 保存充值金额
	sMoneyInfo.fMoney = fAmount;
	// 判断是否保存成功
	if (TRUE == doAddMoney(aName, aPwd, &sMoneyInfo))
	{
		// 提示充值信息
		printf("-----***-----充值信息如下-----***-----\n");
		// 输出表格头
		getLeftAlignFormat(fmt, CARD_NO_WIDTH, "卡号");
		printf(fmt, "卡号"); 
		getLeftAlignFormat(fmt, AMOUNT_WIDTH, "充值金额");
		printf(fmt, "充值金额");
		getLeftAlignFormat(fmt, BALANCE_WIDTH, "余额");
		printf(fmt, "余额");
		printf("\n");
		// 输出充值卡信息
		printf("%-*s%-*.2f%-*.2f\n",
    	CARD_NO_WIDTH, sMoneyInfo.aCardName,
    	AMOUNT_WIDTH, sMoneyInfo.fMoney,
    	BALANCE_WIDTH, sMoneyInfo.fBalance);
		printf("--------------------------------------\n");
	}
	else
	{
		printf("-------充值失败！-------\n");
	}
}

// 函数名:refundMoney
// 功能:退费
// 参数:void
// 返回值:void
void refundMoney()
{
	char aName[18] = {0}; // 卡号
	char aPwd[8] = {0};	  // 密码
	int nResult = -1;	  // 退费结果
	MoneyInfo sMoneyInfo; // 退费信息

	printf("请输入退费卡号(长度1~18):");
	scanf("%18s", aName);
	// 判断输入的卡号是否符合要求
	if (getSize(aName) >= 18)
	{
		printf("输入的卡号长度超过最大值！\n");
		return;
	}

	printf("请输入退费密码(长度1~8):");
	scanf("%8s", aPwd);
	// 判断输入的密码是否符合要求
	if (getSize(aPwd) >= 8)
	{
		printf("输入的密码长度超过最大值！\n");
		return;
	}

	// 进行退费
	nResult = doRefundMoney(aName, aPwd, &sMoneyInfo);
	// 根据退费结果，提示不同信息
	switch (nResult)
	{
	case 0:
	{
		printf("退费失败！\n");
		break;
	}
	case 1:
	{
		// 提示退费信息
		printf("------***----退费信息如下----***------\n");
		getLeftAlignFormat(fmt, CARD_NO_WIDTH, "卡号");
		printf(fmt, "卡号"); 
		getLeftAlignFormat(fmt, AMOUNT_WIDTH, "退费金额");
		printf(fmt, "退费金额");
		getLeftAlignFormat(fmt, BALANCE_WIDTH, "余额");
		printf(fmt, "余额");
		printf("\n");
		// 输出充值卡信息
		printf("%-*s%-*.2f%-*.2f\n",
    	CARD_NO_WIDTH, sMoneyInfo.aCardName,
    	AMOUNT_WIDTH, sMoneyInfo.fMoney,
    	BALANCE_WIDTH, sMoneyInfo.fBalance);
		printf("--------------------------------------\n");
		break;
	}
	case 2:
	{
		printf("------该卡正在使用或已经注销！------\n");
		break;
	}
	case 3:
	{
		printf("------卡余额不足！------\n");
		break;
	}
	default:
	{
		break;
	}
	}
}

// 函数名:annul
// 功能:注销卡
// 参数:void
// 返回值:void
void annul()
{
	Card card;

	printf("请输入注销卡卡号(长度1~18):");
	scanf("%18s", card.aName);
	printf("请输入密码(长度1~8):");
	scanf("%8s", card.aPwd);

	if (FALSE == annulCard(&card))
	{
		printf("---------注销卡失败---------\n");
		return;
	}
	else
	{
		// 提示注销的信息
		printf("---***---注销信息如下---***---\n");
		// 输出表格头
		getLeftAlignFormat(fmt, CARD_NO_WIDTH, "卡号");
		printf(fmt, "卡号"); 
		getLeftAlignFormat(fmt, AMOUNT_WIDTH, "退款金额");
		printf(fmt, "退款金额");
		printf("\n");
		// 输出数据信息
		printf("%-*s%-*.2f\n",
    	CARD_NO_WIDTH, card.aName,
    	AMOUNT_WIDTH, card.fBalance);
		printf("------------------------------\n");

		return;
	}
}



int calcDisplayWidth(const char *str) {
    int width = 0;
    while (*str) {
        unsigned char b = (unsigned char)*str;
        int char_len = 0;
        uint32_t codepoint = 0;

        // 解析 UTF-8 字符
        if (b < 0x80) { // 1字节字符
            char_len = 1;
            codepoint = b;
        } else if ((b & 0xE0) == 0xC0) { // 2字节字符
            char_len = 2;
            if (str[1] == '\0') break;
            codepoint = ((b & 0x1F) << 6) | (str[1] & 0x3F);
        } else if ((b & 0xF0) == 0xE0) { // 3字节字符（中文）
            char_len = 3;
            if (str[1] == '\0' || str[2] == '\0') break;
            codepoint = ((b & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
        } else if ((b & 0xF8) == 0xF0) { // 4字节字符
            char_len = 4;
            if (str[1] == '\0' || str[2] == '\0' || str[3] == '\0') break;
            codepoint = ((b & 0x07) << 18) | ((str[1] & 0x3F) << 12) 
                       | ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
        } else { // 非法字符
            char_len = 1;
            codepoint = 0;
        }

        // 判断是否为中文字符（Unicode 范围）
        if (
            (codepoint >= 0x4E00 && codepoint <= 0x9FFF) ||   // 基本汉字
            (codepoint >= 0x3400 && codepoint <= 0x4DBF) ||   // 扩展A
            (codepoint >= 0x20000 && codepoint <= 0x2A6DF) || // 扩展B
            (codepoint >= 0x2A700 && codepoint <= 0x2B73F) || // 扩展C
            (codepoint >= 0x2B740 && codepoint <= 0x2B81F) || // 扩展D
            (codepoint >= 0x2B820 && codepoint <= 0x2CEAF) || // 扩展E
            (codepoint >= 0xF900 && codepoint <= 0xFAFF) ||   // 兼容表意文字
            (codepoint >= 0x2F800 && codepoint <= 0x2FA1F)    // 兼容补充
        ) {
            width += 2; // 中文字符宽度为2
        } else {
            width += 1; // 其他字符宽度为1
        }

        str += char_len; // 移动到下一个字符
    }
    return width;
}


// 此函数调用可以解决utf-8编码下中文与英文显示列宽不同问题
// 调用格式为getLeftAlignFormat（fmt缓存区，#目标长度，“所输入内容”）
// 用来生成左对齐格式字符串
void getLeftAlignFormat(char *fmt, int targetWidth, const char *str) {
    int actualWidth = calcDisplayWidth(str);
    int padSpaces = targetWidth - actualWidth;
    if (padSpaces < 0) padSpaces = 0;

    // 生成格式字符串（如 "%-12s"）
    sprintf(fmt, "%%-%ds", (int)strlen(str) + padSpaces);
}


// // 函数名cardCount
// // 功能：统计总共有多少卡，有多少可用卡，把所有的卡号信息与状态信息输出
// // 参数：void
// // 返回值：void
// void cardCount() {
//     int total = 0;
//     int available = 0;
//     int index = 0; // 用于接收queryCards返回的实际卡数量

//     // 调用queryCards获取所有卡（pName传空字符串匹配所有卡）
//     Card *pCards = queryCards("", &index); 

//     if (pCards == NULL || index == 0) {
//         printf("--- 没有找到任何卡信息 ---\n");
//         return;
//     }

//     printf("\n------ 卡状态统计 ------\n");

//     // 遍历所有卡
//     for (int i = 0; i < index; i++) {
//         total++;

//         // 状态字段为nStatus，0,1表示可用.3,4为不可用
//         if (pCards[i].nStatus == 0) { 
//             available++;
//         }
// 		if (pCards[i].nStatus == 1) { 
//             available++;
//         }

//         // 输出卡信息（卡号字段为aName）
//         printf("卡号：%-20s 状态：", pCards[i].aName);
//         switch(pCards[i].nStatus) {
//             case 0: 
//                 printf("可用,未上机\n");
//                 break;
//             case 1: 
//                 printf("可用,正在上机\n");
//                 break;
//             case 2: 
//                 printf("已注销\n");
//                 break;
// 			case 3: 
//                 printf("失效\n");
//                 break;
//             default:
//                 printf("未知状态(%d)\n", pCards[i].nStatus);
//         }
//     }

//     printf("\n总计:%d 张卡\n可用卡:%d 张\n", total, available);

//     // 释放queryCards分配的内存
//     free(pCards);
// }
void cardCount() {
    int choice;
    do {
        printf("\n\t\t*查询统计*\n");
        printf("\t1. 按卡号和时间段查询消费记录\n");
        printf("\t2. 按时间段统计总营业额\n");
        printf("\t3. 统计年度分月营业额\n");
        printf("\t0. 返回主菜单\n");
        printf("请选择操作: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                queryConsumption();
                break;
            case 2:
                totalRevenue();
                break;
            case 3:
                monthlyRevenue();
                break;
            case 0:
                printf("返回主菜单...\n");
                break;
            default:
                printf("无效选择，请重新输入！\n");
        }
    } while (choice != 0);
}


// 以下进行拓展功能实现！
// 辅助函数：检查时间是否在指定范围内
int isTimeInRange(time_t checkTime, time_t startTime, time_t endTime) {
    return (checkTime >= startTime) && (checkTime <= endTime);
}

// 查询消费记录
void queryConsumption() {
    char cardName[30];
    char startTimeStr[20];
    char endTimeStr[20];
    
    printf("请输入卡号: ");
    scanf("%s", cardName);
    printf("请输入起始时间(格式: YYYY-MM-DD HH:MM): ");
    getchar(); // 消耗换行符
    fgets(startTimeStr, sizeof(startTimeStr), stdin);
    startTimeStr[strcspn(startTimeStr, "\n")] = 0; // 移除换行符
    printf("请输入结束时间(格式: YYYY-MM-DD HH:MM): ");
    fgets(endTimeStr, sizeof(endTimeStr), stdin);
    endTimeStr[strcspn(endTimeStr, "\n")] = 0; // 移除换行符
    
    time_t startTime = stringToTime(startTimeStr);
    time_t endTime = stringToTime(endTimeStr);
    
    // 验证时间范围
    if (startTime == (time_t)-1 || endTime == (time_t)-1) {
        printf("时间格式错误！\n");
        return;
    }
    if (difftime(endTime, startTime) <= 0) {
        printf("结束时间必须晚于开始时间！\n");
        return;
    }
    
    // 获取计费记录数量
    int billingCount = getBillingCount(BILLINGPATH);
    if (billingCount <= 0) {
        printf("没有找到计费记录！\n");
        return;
    }
    
    // 读取所有计费记录
    Billing* billings = (Billing*)malloc(sizeof(Billing) * billingCount);
    if (!readBilling(billings, BILLINGPATH)) {
        printf("读取计费记录失败！\n");
        free(billings);
        return;
    }
    
    printf("\n消费记录查询结果 (卡号: %s, 时间段: %s 至 %s)\n", cardName, startTimeStr, endTimeStr);
    printf("---------------------------------------------------------------------\n");
    printf("%-20s %-20s %-20s %-10s\n", "卡号", "上机时间", "下机时间", "消费金额");
    printf("---------------------------------------------------------------------\n");
    
    int found = 0;
    for (int i = 0; i < billingCount; i++) {
        if (billings[i].nDel == 0 && billings[i].nStatus == 1 && // 有效且已结算的记录
            strcmp(billings[i].aCardName, cardName) == 0 &&
            isTimeInRange(billings[i].tEnd, startTime, endTime)) {
            
            char startStr[20], endStr[20];
            timeToString(billings[i].tStart, startStr);
            timeToString(billings[i].tEnd, endStr);
            
            printf("%-20s %-20s %-20s ¥%-10.2f\n", 
                   billings[i].aCardName, startStr, endStr, billings[i].fAmount);
            found = 1;
        }
    }
    
    if (!found) {
        printf("没有找到符合条件的消费记录！\n");
    }
    
    printf("---------------------------------------------------------------------\n");
    free(billings);
}

// 统计总营业额
void totalRevenue() {
    char startTimeStr[20];
    char endTimeStr[20];
    
    printf("请输入起始时间(格式: YYYY-MM-DD HH:MM): ");
    getchar(); // 消耗换行符
    fgets(startTimeStr, sizeof(startTimeStr), stdin);
    startTimeStr[strcspn(startTimeStr, "\n")] = 0; // 移除换行符
    printf("请输入结束时间(格式: YYYY-MM-DD HH:MM): ");
    fgets(endTimeStr, sizeof(endTimeStr), stdin);
    endTimeStr[strcspn(endTimeStr, "\n")] = 0; // 移除换行符
    
    time_t startTime = stringToTime(startTimeStr);
    time_t endTime = stringToTime(endTimeStr);
    
    // 验证时间范围
    if (startTime == (time_t)-1 || endTime == (time_t)-1) {
        printf("时间格式错误！\n");
        return;
    }
    if (difftime(endTime, startTime) <= 0) {
        printf("结束时间必须晚于开始时间！\n");
        return;
    }
    
    // 获取计费记录数量
    int billingCount = getBillingCount(BILLINGPATH);
    if (billingCount <= 0) {
        printf("没有找到计费记录！\n");
        return;
    }
    
    // 读取所有计费记录
    Billing* billings = (Billing*)malloc(sizeof(Billing) * billingCount);
    if (!readBilling(billings, BILLINGPATH)) {
        printf("读取计费记录失败！\n");
        free(billings);
        return;
    }
    
    double totalAmount = 0.0;
    int count = 0;
    
    for (int i = 0; i < billingCount; i++) {
        if (billings[i].nDel == 0 && billings[i].nStatus == 1 && // 有效且已结算的记录
            isTimeInRange(billings[i].tEnd, startTime, endTime)) {
            
            totalAmount += billings[i].fAmount;
            count++;
        }
    }
    
    printf("\n营业额统计结果 (时间段: %s 至 %s)\n", startTimeStr, endTimeStr);
    printf("--------------------------------------------------\n");
    printf("总消费记录数: %d\n", count);
    printf("总营业额: ¥%.2f\n", totalAmount);
    printf("--------------------------------------------------\n");
    
    free(billings);
}

// 统计月度营业额
void monthlyRevenue() {
    int year;
    printf("请输入要统计的年份(YYYY): ");
    scanf("%d", &year);
    
    // 获取计费记录数量
    int billingCount = getBillingCount(BILLINGPATH);
    if (billingCount <= 0) {
        printf("没有找到计费记录！\n");
        return;
    }
    
    // 读取所有计费记录
    Billing* billings = (Billing*)malloc(sizeof(Billing) * billingCount);
    if (!readBilling(billings, BILLINGPATH)) {
        printf("读取计费记录失败！\n");
        free(billings);
        return;
    }
    
    // 初始化月份营业额数组
    double monthlyAmount[12] = {0};
    int monthlyCount[12] = {0};
    
    for (int i = 0; i < billingCount; i++) {
        if (billings[i].nDel == 0 && billings[i].nStatus == 1) {
            // 获取时间结构
            struct tm* timeinfo = localtime(&(billings[i].tEnd));
            int recordYear = timeinfo->tm_year + 1900;
            int month = timeinfo->tm_mon;
            
            if (recordYear == year && month >= 0 && month < 12) {
                monthlyAmount[month] += billings[i].fAmount;
                monthlyCount[month]++;
            }
        }
    }
    
    printf("\n%d年度分月营业额统计\n", year);
    printf("--------------------------------------------------\n");
    printf("月份     消费记录数       营业额\n");
    printf("--------------------------------------------------\n");
    
    const char* monthNames[] = {"一月", "二月", "三月", "四月", "五月", "六月",
                               "七月", "八月", "九月", "十月", "十一月", "十二月"};
    
    for (int i = 0; i < 12; i++) {
        printf("%-8s %-12d ¥%-12.2f\n", monthNames[i], monthlyCount[i], monthlyAmount[i]);
    }
    
    printf("--------------------------------------------------\n");
    free(billings);
}
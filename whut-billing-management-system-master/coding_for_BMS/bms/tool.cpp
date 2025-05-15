#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 函数名: timeToString
// 功能: 将 time_t 时间戳转换为 "年-月-日 时:分" 格式的字符串
// 参数: time_t timestamp - 输入时间戳
//      char* buffer     - 输出缓冲区（建议至少分配20字节）
// 返回值: void
void timeToString(time_t timestamp, char* buffer) {
    struct tm timeinfo = {};  // 栈上分配，无需手动释放
    
    // 使用线程安全版本转换本地时间
    if (localtime_r(&timestamp, &timeinfo) == nullptr) {
        // 错误处理
        buffer[0] = '\0';  // 返回空字符串
        return;
    }

    // 格式化为字符串
    strftime(buffer, 20, "%Y-%m-%d %H:%M", &timeinfo);
}

// 函数名:stringToTime
// 功能:将格式为“年-月-日 时：分”的字符串转化为time_t类型时间
// 参数:char* pTime:“年-月-日 时：分”格式的字符串
// 返回值: time_t:转化后的时间类型，从1970年到改时间的秒数
time_t stringToTime(char *pTime)
{
	struct tm tm1;
	time_t time1;

	sscanf(pTime, "%d-%d-%d %d:%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min);
	tm1.tm_year -= 1900; // 年份为从1970年开始
	tm1.tm_mon -= 1;
	tm1.tm_sec = 0;
	tm1.tm_isdst = -1;

	time1 = mktime(&tm1);

	return time1;
}
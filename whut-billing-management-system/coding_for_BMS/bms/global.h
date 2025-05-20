#pragma once

#define FALSE 0       // 表示失败
#define TRUE 1        // 表示成功
#define UNUSE 2       // 卡不能使用
#define ENOUGHMONEY 3 // 余额不足

#define UNIT 15    // 最小收费单元（分钟）
#define CHARGE 0.5 // 每个计费单元收费(RMB:元）

#define CARDPATH "data/card.ams"       // 卡信息保存路径
#define BILLINGPATH "data/billing.ams" // 计费信息保存路径
#define MONEYPATH "data/money.ams"     // 充值退费信息保存路径

// 列宽常量
#define CARD_NO_WIDTH      18  // 卡号（支持1-18长度）
#define PWD_WIDTH          20  // 密码
#define STATUS_WIDTH       8  // 状态
#define INIT_BALANCE_WIDTH 12  // 开卡金额（如：10000.00）
#define BALANCE_WIDTH      12  // 余额
#define AMOUNT_WIDTH       16  // 充值/退费
#define TIME_WIDTH         20  // 时间（格式：YYYY-MM-DD HH:MM）
#define CONSUME_WIDTH      16  // 消费金额
#define COL_TOTAL_USE      16  // 累计金额列宽 
#define COL_USE_COUNT      16  // 使用次数列宽
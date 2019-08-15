#ifndef __MSHELL_CONFIG_H__
#define __MSHELL_CONFIG_H__

#include "types.h"

// <<< Use Configuration Wizard in Context Menu >>>
//<s> 版本
#define MSHELL_VERSION        "mshell v1.0"
//<s> 终端前导字符
#define MSHELL_PROMPT         "msh>"
//<o> 命令长度
#define MSHELL_CMD_SIZE       50
//<o> 命令参数个数
#define MSHELL_CMD_ARG_MAX    5
//<o> 命令参数长度
#define MSHELL_CMD_ARG_LEN    10

//<c>回显
#define MSHELL_USING_ECHO
//</c>

//<e> 支持路径
#define MSHELL_USING_DIR       1
//<o> 目录最大长度
#define MSHELL_DIR_MAX 64
//</e>

//<c> 支持登录
//#define MSHELL_USING_LOGIN
//</c>

//<e> 支持历史记录
#define MSHELL_USING_HISTORY         1
//<o> 条数 
#define MSHELL_HISTORY_LINES  10
//</e>

#endif

#define mshell_printf rt_kprintf

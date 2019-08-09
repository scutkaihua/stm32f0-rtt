#ifndef __MSHELL_CONFIG_H__
#define __MSHELL_CONFIG_H__

#include "types.h"


// <<< Use Configuration Wizard in Context Menu >>>
//<s> 终端前导字符
#define MSHELL_PROMPT         "msh>>"
//<o> 命令长度
#define MSHELL_CMD_SIZE       50

//<c> 支持路径
#define MSHELL_USING_DIR 
//</c>

//<c> 支持登录
#define MSHELL_USING_LOGIN
//</c>

//<e> 支持历史记录
#define MSHELL_HISTORY         0
//<o> 条数 
#define MSHELL_HISTORY_LINES  10
//</e>

#endif


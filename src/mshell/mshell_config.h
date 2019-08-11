#ifndef __MSHELL_CONFIG_H__
#define __MSHELL_CONFIG_H__

#include "types.h"

// <<< Use Configuration Wizard in Context Menu >>>
//<s> �ն�ǰ���ַ�
#define MSHELL_PROMPT         "msh>"
//<o> �����
#define MSHELL_CMD_SIZE       50

//<c>����
#define MSHELL_USING_ECHO
//</c>

//<e> ֧��·��
#define MSHELL_USING_DIR       1
//<o> Ŀ¼��󳤶�
#define MSHELL_DIR_MAX 64
//</e>

//<c> ֧�ֵ�¼
//#define MSHELL_USING_LOGIN
//</c>

//<e> ֧����ʷ��¼
#define MSHELL_USING_HISTORY         1
//<o> ���� 
#define MSHELL_HISTORY_LINES  10
//</e>

#endif

#define mshell_printf rt_kprintf
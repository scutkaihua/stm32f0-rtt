#ifndef __MSHELL_CONFIG_H__
#define __MSHELL_CONFIG_H__

#include "types.h"

// <<< Use Configuration Wizard in Context Menu >>>
//<s> �汾
#define MSHELL_VERSION        "mshell v1.0"
//<s> �ն�ǰ���ַ�
#define MSHELL_PROMPT         "sh"
//<o> �����
#define MSHELL_CMD_SIZE       50
//<o> �����������
#define MSHELL_CMD_ARG_MAX    5
//<o> �����������
#define MSHELL_CMD_ARG_LEN    20

//<c>����
#define MSHELL_USING_ECHO
//</c>

//<e> ֧��·��
#define MSHELL_USING_DIR       1
//<o> Ŀ¼��󳤶�
#define MSHELL_DIR_MAX 64
//</e>

//<e> ֧�ֵ�¼
#define MSHELL_USING_LOGIN  1
//<o>��¼��ʱ(s)
#define MSHELL_LOGIN_TIMEOUT 180
//<o>�û�����󳤶�
#define MSHELL_LOGIN_NAME_MAX 10
//<o>������󳤶�
#define MSHELL_LOGIN_PW_MAX 10
//</e>

//<e> ֧����ʷ��¼
#define MSHELL_USING_HISTORY    1
//<o> ���� 
#define MSHELL_HISTORY_LINES  10
//</e>

#endif

//<c> ʹ��rt_kprintf��Ϊ��ӡ���
//#define Using_rt_printf_as_output
//void rt_kprintf(const char *fmt, ...);
//#define mshell_printf rt_kprintf
//</c>
#ifndef Using_rt_printf_as_output
extern void mshell_printf(const char *fmt, ...);
#endif

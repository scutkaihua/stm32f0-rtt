#ifndef __M_SHELL_H__
#define __M_SHELL_H__

#include "mshell_config.h"

/******************************************************************************
******************************************************************************/
/*��������*/
typedef struct{
	
	#if MSHELL_USING_DIR>0
  char*dir;               //·��֧��
	#endif
	
	char*cmd;               //����
	void*function;          //�����
	char*info;              //������Ϣ
}Mshell_Cmd;

#if MSHELL_USING_DIR >0

/*·������*/
typedef struct{

}Mshell_Dir;
#endif

#if MSHELL_USING_LOGIN >0
/*��¼����*/
typedef struct{


}Mshell_Login;
#endif


/******************************************************************************
// ���������
******************************************************************************/
/*dir:·��  cmd:�������ʱ����  f:����� info:����˵��*/
#if MSHELL_USING_DIR>0
#define Export(dir,cmd,f,info) Mshell_Cmd const MshellCmd_##cmd __attribute__((section("MSHELL")))  __attribute__((used)) = \
       {(char*)dir,(char*)#cmd,(void*)f,(char*)info};
#else 
#define Export(dir,cmd,f,info) Mshell_Cmd const MshellCmd_##cmd __attribute__((section("MSHELL")))  __attribute__((used)) = \
       {(char*)#cmd,(void*)f,(char*)info};			 
#endif

			 
/******************************************************************************
// ȫ�ֺ���
******************************************************************************/			 
void mshell(char data);
void mshell_init(void);

#endif


#ifndef __M_SHELL_H__
#define __M_SHELL_H__

#include "mshell_config.h"

/******************************************************************************
******************************************************************************/
/*命令类型*/
typedef struct{
	
	#if MSHELL_USING_DIR>0
  char*dir;               //路径支持
	#endif
	
	char*cmd;               //命令
	void*function;          //命令函数
	char*info;              //命令信息
}Mshell_Cmd;

#if MSHELL_USING_DIR >0

/*路径控制*/
typedef struct{

}Mshell_Dir;
#endif

#if MSHELL_USING_LOGIN >0
/*登录控制*/
typedef struct{


}Mshell_Login;
#endif


/******************************************************************************
// 定义命令函数
******************************************************************************/
/*dir:路径  cmd:命令调用时名称  f:命令函数 info:命令说明*/
#if MSHELL_USING_DIR>0
#define Export(dir,cmd,f,info) Mshell_Cmd const MshellCmd_##cmd __attribute__((section("MSHELL")))  __attribute__((used)) = \
       {(char*)dir,(char*)#cmd,(void*)f,(char*)info};
#else 
#define Export(dir,cmd,f,info) Mshell_Cmd const MshellCmd_##cmd __attribute__((section("MSHELL")))  __attribute__((used)) = \
       {(char*)#cmd,(void*)f,(char*)info};			 
#endif

			 
/******************************************************************************
// 全局函数
******************************************************************************/			 
void mshell(char data);
void mshell_init(void);

#endif


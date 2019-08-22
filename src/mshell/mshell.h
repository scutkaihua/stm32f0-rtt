#ifndef __M_SHELL_H__
#define __M_SHELL_H__

#include "mshell_config.h"

/******************************************************************************
		匿名
******************************************************************************/
//合并两个名称 
#define MSHXY(x,y) x##y
#define MSHCXY(x,y) MSHXY(x,y)

//合并三个名称
#define MSHXYLINE(class,name)    class##_##name##_line

//定义匿名名(变量名)
#define MSH_UnknownName(class,name)       MSHCXY(MSHXYLINE(class,name),__LINE__)
	
/*段重定向用到的宏*/
#define msh_used          						__attribute__((used))				/*used，表示不优化*/
#define mshsection(x)                 __attribute__((section(x))) /*段重定向*/

/******************************************************************************
******************************************************************************/
/*命令处理函数,实际上为 long(*fn)(...),入参为省略参数，默认为NULL*/
typedef int (*syscall_func)();

/*命令类型*/
typedef struct{
	
	#if MSHELL_USING_DIR>0
  char*dir;               //路径支持
	#else
	int grade;              //命令权限等级
	#endif
	
	char*name;              //命令	
	char*desc;              //命令信息
	syscall_func func;      //命令函数
}Mshell_Cmd;


/******************************************************************************
// 定义命令函数
******************************************************************************/
/*dir:路径  cmd:命令调用时名称  f:命令函数 info:命令说明*/
#if MSHELL_USING_DIR>0
#define Export(dir,cmd,f,i) Mshell_Cmd const MshellCmd_##cmd msh_used mshsection(#dir) = \
       {(char*)#dir,(char*)#cmd,(char*)i,(syscall_func)&f};
#else 
#define Export(grade,cmd,f,i) Mshell_Cmd const MshellCmd_##cmd __attribute__((section("MSHELL")))  __attribute__((used)) = \
       {grade,(char*)#cmd,(char*)i,(syscall_func)&f};			 
#endif

/******************************************************************************
// 目录支持
******************************************************************************/
#if MSHELL_USING_DIR>0

/*路径控制*/
typedef struct{
	const char* dir;   /*目录*/
	const int   grade; /*目录权限等级，当登录等级>=目录等级，才可以访问目录的命令*/
	
	/*命令集合*/
	Mshell_Cmd*start;
	Mshell_Cmd*end;
}Mshell_Dir;

#define Export_DIR(dir,name,grade)    \
	extern const int name##$$Base; \
  extern const int name##$$Limit; \
	const Mshell_Dir MSH_UnknownName(UnkownDir,name) msh_used mshsection("FSymDir")= \
	{ \
	dir, \
	grade, \
	(Mshell_Cmd*)&name##$$Base, \
	(Mshell_Cmd*)&name##$$Limit \
	} ;
#else
#define Export_DIR(dir,name,grade)  static const int name = grade;
#endif

			 
/******************************************************************************
// 全局函数
******************************************************************************/			 
void mshell(char data);

#endif


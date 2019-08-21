
#include "mshell.h"
#include "string.h"
#include "stdio.h"
/*************************************************************************
*			通用命令
**************************************************************************/
extern char mshell_dir[];
extern long list_dir(int g,char*sh_dir,char*indir,char*dir);
extern char*dir_parse(char*sdir,char*dir,char*out);

extern int mshell_grade(void);

Export_DIR("/",root,0);

/*列表当前目录，列表所有变量*/
static long list(char*indir)
{
	#if MSHELL_USING_DIR>0
  char dir[MSHELL_DIR_MAX] = {0};
	return list_dir(mshell_grade(),mshell_dir,indir,dir);
	#else 
	extern const int MSHELL$$Base;
	extern const int MSHELL$$Limit;
	Mshell_Cmd*cs = (Mshell_Cmd*)&MSHELL$$Base;
	Mshell_Cmd*ce = (Mshell_Cmd*)&MSHELL$$Limit;
	mshell_printf("-- 命令------------ 描述------------------");
	while(cs != ce)
	{
		if(mshell_grade()>=cs->grade)
		 mshell_printf("\n%-16s -- %s", cs->name, cs->desc);
		 cs++;
	}
	#endif
}

/*版本*/
static long ver(void)
{
	mshell_printf(MSHELL_VERSION);
	return 1;
}

/*清屏*/
static long cls(void)
{
	mshell_printf("\033[2J");
	return 1;
}
extern long cd(char*in_dir);
Export(root,ls,list,        	  "ls 或者 ls() 列表内容")
Export(root,ver,ver,        	  "ver 或者 ver() 版本号")
Export(root,cls,cls,        	  "cls 或者 cls() 清屏")
#if MSHELL_USING_DIR>0
Export(root,cd,cd,							"cd string  或者 cd(string)")
#endif

#if MSHELL_USING_LOGIN > 0
extern int login(char*line);
extern int logout(void);
Export(root,login,login,        "login xxx xxxx 登录")
Export(root,logout,logout,      "logout 登出")
#endif

/*finsh优先运行，特殊命令*/
int finsh_run_line_first(char*line)
{
	if(strlen(line)==0)return 1;
	mshell_printf("\n");
  /*================cd===============================*/
	#if MSHELL_USING_DIR>0
	if( memcmp("cd ",line,3)==0 )                      //有参cd
	{
		char dir[MSHELL_DIR_MAX] = {0};
		sscanf((const char*)line,"cd %s",dir);//读取参数
		if(strlen(dir)>0){dir[strlen(dir)]=0;}
		cd(dir);//进入目录
		return 1;
	}else if( memcmp("cd",line,2)==0 ){cd(0); return 1;}//无参cd
	#endif
	/*================ls===============================*/
	if(strcmp("ls;",line)==0 || strcmp("ls",line)==0 )
	{
		list(NULL);
		return 1;
	}else if(memcmp("ls ",line,strlen("ls ")) == 0){
		char dir[MSHELL_DIR_MAX] = {0};
		sscanf(line,"ls %s",dir);
		if(strlen(dir)>0){dir[strlen(dir)]=0;}
		list(dir);
	  return 1;
	}
	/*================ver===============================*/
	if((strcmp("ver",line)==0)||(strcmp("ver()",line)==0))
	{
		ver();return 1;
	}
	/*================cls===============================*/
	if((strcmp("cls",line)==0)||(strcmp("cls()",line)==0))
	{
		cls();return 1;
	}
	/*================login=============================*/
	#if MSHELL_USING_LOGIN > 0
	if(memcmp("login",line,5)==0){login(line);return 2;}
	if(memcmp("logout",line,6)==0){logout();return 1;}
	#endif
	return 0;
}


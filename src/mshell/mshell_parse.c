#include "mshell.h"
#include "string.h"

extern char mshell_dir[];
extern char*dir_parse(char*sdir,char*dir,char*out);
extern int finsh_grade;
/******************************************************************************
// 解释参数
// 只支持几种参数
  1.""   字符串
	2.long 整型
	3.0x   Hex码 ==>转成 整型
******************************************************************************/
long args[MSHELL_CMD_ARG_MAX][MSHELL_CMD_ARG_LEN/sizeof(long)];
S8 argstep=0;
S8 argline=0;
S8 argoffset=0;
void mshell_input_arg_parse(char ch)
{
	/*参数形式("xxxxxx(,",12345, 5678,"123213",0x123)*/
	/**/
	switch(argstep)
	{
		case 0:
			if(ch=='(')argstep++;//开始参数
			break;
		case 2:
			
		/*-----------字符串-------------*/
		case 10:
	
		/*-----------0x 0X16进制--------*/
		case 20:
			
		/*-----------10进制数字---------*/
			
	}


}




int msell_parse_args(char*la)
{
	if(la==NULL)return 0;
	int i=0;
	if(*la=='(')la++;
	{
		
		int clen = 0;	
		char*token = strtok(la, " ");
		memset(args,0,sizeof(args));
		/* 继续获取其他的子字符串 */
		 while( token != NULL ) {
       int len = strlen(token);
			 if(len>0)
			 {
				 clen = len>MSHELL_CMD_ARG_LEN?MSHELL_CMD_ARG_LEN:len;
			   memcpy(args[i],token,clen);
			 }
			 i++;
			 la+=len+1;
			 token = strtok(la, " ");
			 if(token==NULL){
				 if(clen>0){
					 char*s = (char*)args[i-1];
					 if(s[clen-1]==')')s[clen-1]=0;//去掉末尾 ')'
				 }
				 return i;
			 }
			 if(i>=MSHELL_CMD_ARG_MAX)return i;
		 }
		return i;
  }
}

/******************************************************************************
// 查找函数
******************************************************************************/
Mshell_Cmd*mshell_cmd_get(char*dir,char*name,int g)
{
#if MSHELL_USING_DIR>0
	extern const int FSymDir$$Base;
	extern const int FSymDir$$Limit;
	Mshell_Dir*dirs = (Mshell_Dir*)&FSymDir$$Base;
	Mshell_Dir*dire = (Mshell_Dir*)&FSymDir$$Limit;
	//当前路径命令
	while(dirs != dire)
	{
		if( (strcmp( dir, dirs->dir ) == 0) && (g>=dirs->grade) )
		{
			Mshell_Cmd*cs = dirs->start;
			Mshell_Cmd*ce = dirs->end;
			while(cs != ce){
				if(strcmp(name,cs->name)==0)
				{
					return cs;
				}
				cs++;
			}
		}
		dirs++;
	}
	return NULL;
#else
	extern const int MSHELL$$Base;
	extern const int MSHELL$$Limit;
	Mshell_Cmd*cs = (Mshell_Cmd*)&MSHELL$$Base;
	Mshell_Cmd*ce = (Mshell_Cmd*)&MSHELL$$Limit;
  while(cs != ce)
	{
		if(strcmp(name,cs->name)==0)
		{
			if(g>=cs->grade)
			{
				return cs;  //返回命令
			}else{
				return NULL;//无权访问
			}
		}
	}
	return NULL;
#endif
}

/******************************************************************************
// 调用函数
******************************************************************************/


long mshell_cmd_call(char*line)
{
	char*out=NULL;
	char*p=NULL;
	char*n=strchr(line,'(');
	char name[MSHELL_CMD_SIZE]={0};
	int a = 0;
	Mshell_Cmd*cmd;
	
	//解析当前路径
#if MSHELL_USING_DIR>0
	char dir[MSHELL_DIR_MAX] = {0};
	p=strrchr(line,'\/');
	
	if(p!=NULL&&p<n){
		*p=0;p++;
		out = dir_parse(mshell_dir,line,dir);
		if(out == NULL)return -1;
	}else{
		p = line;
		out = dir_parse(mshell_dir,"./",dir);
	}
	
#else
	p=line;
#endif
	
	//解析名字
	memcpy(name,p,(n==NULL)?strlen(p):(n-p));
  cmd = mshell_cmd_get(out,name,finsh_grade);
	if(cmd==NULL)return -1;
	
	//解析参数,调用命令
	//mshell_printf("\n");
	if(n==NULL)return cmd->func(0);
	else{
		a = msell_parse_args(n+1);
		switch(a)
		{
			case 0:return cmd->func();
			case 1:return cmd->func(args[0]);
			case 2:return cmd->func(args[0],args[1]);
			case 3:return cmd->func(args[0],args[1],args[2]);
			case 4:return cmd->func(args[0],args[1],args[2],args[3]);
			case 5:return cmd->func(args[0],args[1],args[2],args[3],args[4]);
			case 6:return cmd->func(args[0],args[1],args[2],args[3],args[4],args[5]);
			default:return -1;
		}
	}
	return -1;
}
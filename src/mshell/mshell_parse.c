#include "mshell.h"
#include "string.h"

extern char mshell_dir[];
extern char*dir_parse(char*sdir,char*dir,char*out);
extern int finsh_grade;

#define is_number(ch)    (ch>='0'&&ch<='9')
#define is_hex(ch)       (is_number(ch)||(ch>='a'&&ch<='f')||(ch>='A'&&ch<='F'))
#define is_8hex(ch)      (ch>='0'&&ch<='7')
char hex_bin(char h)
{
	if(h>='0'&&h<='9')return h-'0';
	if(h>='a'&&h<='f')return h-'a'+10;
	if(h>='A'&&h<='F')return h-'A'+10;
	return 0;
}
/*-1: error   >=0:转换后的数值*/
int is_jinzhi(char ch,char jinzhi)
{
	switch(jinzhi)
	{
		case 2:if(ch!='0'&& ch!='1')return -1;else return ch-'1';//2进制
		case 8:if(ch <'0'&& ch >'7')return -1;else return ch-'0';//8进制
		case 16:if(!is_hex(ch))return 1;else return hex_bin(ch);  //16进制
		case 10:                                           //10进制
			     if(!is_number(ch))return -1;
					 else return ch-'0';
	}
	return 0;
}
	

/******************************************************************************
// 解释参数
// 只支持几种参数
  1.""   字符串
	2.long 整型
	3.0x   Hex码 ==>转成 整型
******************************************************************************/
typedef struct{
	char*data;
	S8 line;
	S8 offset;
	S8 step;
	//============数值转换时用=======//
	S8 sign;//符号位
	S8 no;//第几个
	long value;
}Token;
long args[MSHELL_CMD_ARG_MAX][MSHELL_CMD_ARG_LEN/sizeof(long)];
Token token={NULL,0,0,0};
/*
* 解析一串字符
* ch     :传入的字符
* return :0 未完成  >0参数个数  <0:解析失败
*/
int mshell_input_arg_parse(char*line)
{
	char ch;
	memset(&token,0,sizeof(token));
	memset(args,0,sizeof(args));
	token.data=(char*)args[0];
	while( (ch=*line++)!='\0')
	{
		/*参数形式("xxxxxx(,",12345, 5678,"123213",0x123)*/
		switch(token.step)
		{
			case 0:if(ch=='('){token.step++;continue;}//等待'('
			case 1:
				if(ch ==' ')continue;
				token.step++;	
			case 2://参数
				if(ch==')')return token.line;                         //参数结束
				if(ch==','&&token.line==0)goto MSHELL_PARSE_ARG_ERROR;//第一个之前参数不能有','
				if(ch==',' || ch==' ')continue;
				if(ch=='"'){token.offset=0; token.step=3;continue;}	                  //字符串开始	
				if(!is_number(ch)&&ch!='-')
					goto MSHELL_PARSE_ARG_ERROR;
				token.step=10;
				token.value=0;
				token.sign=1;
				token.no=10;//默认10进制
			/*-----------数字转换-----------*/
			//16:0x  8:0o 2:0b
			case 10:
			{
				if(ch==' ')continue;
				token.offset++;
				if(ch=='-'){token.sign=-1;continue;}
				if(ch=='0'&&token.offset>=1){token.no=11; continue;}
				if(token.no==11)
				{
					if(token.sign==-1)goto MSHELL_PARSE_ARG_ERROR;
					switch(ch)
					{
						case 'x':case 'X':token.no = 16;continue;
						case 'b':case 'B':token.no = 2;continue;
						case 'O':case 'o':token.no = 8;continue;
						default:goto MSHELL_PARSE_ARG_ERROR;
					}
				}
				//本参数结束
				if(ch==')' || ch==',')
				{
					args[token.line][0] = token.value*token.sign;
					token.line++;
					if(ch==','){token.step=2;continue;}
					return token.line;
				}
				//参数是否格式错
				{
					int result = is_jinzhi(ch,token.no);
					if( result <0)
					goto MSHELL_PARSE_ARG_ERROR;
					ch = (char)result;
			  }
				//计数当前值
				token.value*=token.no;
				token.value+=ch;
			}
			continue;	
			/*-----------字符串-------------*/
			case 3:
			{
				if(ch=='\\'){token.step=4;continue;}                           //转义开始
				if(ch=='"'){
					token.offset=0;
					token.line++;
					token.data=(char*)args[token.line];
					token.step=2;
					continue;
				}//下一行
				else
					token.data[token.offset++]=ch;
				continue;
			}
			case 4:
			{		
				/* 转义 
				        \a 响铃  \b 退格  \t 制表  \n 换行  \v 纵向制表    \f 换页  \r 回车  \e esc   \0 0  \" "   \\ \   \? ?   \' '  \ddd 八进制  \xhh 十六进制
				*/
				switch(ch)
				{
					case 'a':ch='\a';break;case 'b':ch='\b';break;case 't':ch='\t';break;case 'n':ch='\n';break;
					case 'v':ch='\v';break;case 'f':ch='\f';break;case 'r':ch='\r';break;case 'e':ch='\e';break;
					case '0':ch='\0';break;case '"':ch='"' ;break;case '\\':ch='\\';break;case '?':ch='\?';break;
					case '\'':ch='\'';break;case 'x':{token.no=token.value=0;token.step=5;continue;}
					default:
						if(is_8hex(ch))goto MSHELL_PARSE_8;
						goto MSHELL_PARSE_ARG_ERROR;
				}
				token.data[token.offset++]=ch;
				token.step=3;continue;
			}
			case 5://16进制转码
			{
				if(!is_hex(ch))goto MSHELL_PARSE_ARG_ERROR;
				token.value<<=4;token.value+=hex_bin(ch);
				token.no++;
				if(token.no==2)
				{
					token.data[token.offset++]=(char)token.value;
					token.no=token.value=0;
					token.step=3;
				}
				continue;
			}
			MSHELL_PARSE_8:
			token.no=token.value=0;token.step=6;
			case 6://8进制转码
			{
				if(!is_8hex(ch))goto MSHELL_PARSE_ARG_ERROR;
				token.value<<=3;token.value+=ch-'0';
				token.no++;
				if(token.no==3)
				{
					token.data[token.offset++]=(char)token.value;
					token.no=token.value=0;
					token.step=3;
				}
				continue;
			}
			default:goto MSHELL_PARSE_ARG_ERROR;
		}
		continue;
  }
	MSHELL_PARSE_ARG_ERROR://参数解析错误
	memset(&token,0,sizeof(token));
	return -1;
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
		if(out == NULL){
			mshell_printf("Dir error.");
			return -1;
		}
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
	if(cmd==NULL){
		mshell_printf("No such cmd.");
		return -1;
	}
	
	//解析参数,调用命令
	//mshell_printf("\n");
	if(n==NULL)return cmd->func(0);
	else{
		a = mshell_input_arg_parse(n);
		switch(a)
		{
			case 0:return cmd->func();
			case 1:return cmd->func(args[0]);
			case 2:return cmd->func(args[0],args[1]);
			case 3:return cmd->func(args[0],args[1],args[2]);
			case 4:return cmd->func(args[0],args[1],args[2],args[3]);
			case 5:return cmd->func(args[0],args[1],args[2],args[3],args[4]);
			case 6:return cmd->func(args[0],args[1],args[2],args[3],args[4],args[5]);
			default:
				if(a==-1)mshell_printf("args error");
			  return -1;
		}
	}
	return -1;
}
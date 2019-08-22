
/******************************************************************************
	mshell_parse.c 
	功能: 解析命令行
	1.支持路径
	2.支持权限
	3.支持两种参数: 字符串 数值(2进制0b 10进制 8进制0O 16进制0x)
******************************************************************************/
#include "mshell.h"
#include "string.h"


extern int mshell_grade(void);

#if MSHELL_USING_DIR>0
extern char mshell_dir[];
extern char*dir_parse(char*sdir,char*dir,char*out);
#endif
/******************************************************************************
		类型宏
******************************************************************************/
typedef enum{
	ARG_TYPE_STRING =0,
	ARG_TYPE_NUMBER =1,
}ARG_TYPE;
#define ARG(o)  ((void*)token.arg[o].value)
	
/*判断数值,hex码,8进制码*/
#define is_number(ch)    (ch>='0'&&ch<='9')
#define is_hex(ch)       (is_number(ch)||(ch>='a'&&ch<='f')||(ch>='A'&&ch<='F'))
#define is_8hex(ch)      (ch>='0'&&ch<='7')
/******************************************************************************
		私有
******************************************************************************/
/*输出hex码对应的值
* h    :输入的字符
*return:对应的hex值
*/
static char hex_bin(char h)
{
	if(h>='0'&&h<='9')return h-'0';
	if(h>='a'&&h<='f')return h-'a'+10;
	if(h>='A'&&h<='F')return h-'A'+10;
	return 0;
}

/* 转换字符ch
* ch    :输入的字符
* jinzhi:对应的进制,支持 2,8,10,16
*return :对应的值,  <0:出错
*/
int is_jinzhi(char ch,char jinzhi)
{
	switch(jinzhi)
	{
		case 2:if(ch!='0'&& ch!='1')return -1;else return ch-'0';//2进制
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
typedef struct{void*value;long data[MSHELL_CMD_ARG_LEN/sizeof(long)];}Arg;//type=1 字符串  2数值
typedef struct{
	char*data; S8 line,offset,step;
	//============数值转换时用=======//
	S8 sign;//符号位
	S8 no;  //第几个
	long value;//用于数据计算缓冲
	//============转换后的参数=======//
	Arg arg[MSHELL_CMD_ARG_MAX];
}Token;
static Token token={NULL,0,0,0};
/*
* 解析一串字符
* ch     :传入的字符
* return :0 未完成  >0参数个数  <0:解析失败
*/
static int mshell_input_arg_parse(char*line)
{
	char ch;
	memset(&token,0,sizeof(token));
	token.data=(char*)token.arg[0].data;
	if(line==NULL)return 0;
	while( ((ch=*line++)!='\0') && (token.line<MSHELL_CMD_ARG_MAX) )
	{
		/*参数形式("xxxxxx(,",12345, 5678,"123213",0x123)*/
		switch(token.step)
		{
			/*-----------等待'('------------*/
			case 0:if(ch=='('){token.step++;continue;}
			case 1:
				if(ch ==' ')continue;
				token.step++;	
			/*-----------参数开始-----------*/
			case 2:
				if(ch==')')return token.line;                         //参数结束
				if(ch==','&&token.line==0)goto MSHELL_PARSE_ARG_ERROR;//第一个之前参数不能有','
				if(ch==',' || ch==' ')continue;                       //','' '不处理
				if(ch=='"'){token.offset=0; token.step=3;continue;}	  //字符串开始	
				if(!is_number(ch)&&ch!='-')                           //非数字，非符号，解析错误
					goto MSHELL_PARSE_ARG_ERROR;
				token.step=10;//进入数字转换
				token.value=0;//缓存值清0
				token.sign=1; //默认符号+
				token.no=10;  //默认10进制
			/*-----------数字转换-----------*/
			//16:0x  8:0o 2:0b
			case 10:
			{
				if(ch==' ')continue;
				if(ch=='-'){token.sign=-1;continue;}
				token.offset++;
				if(ch=='0'&& token.offset==1){token.no=11; continue;}
				if(token.no==11)
				{
					//if(token.sign==-1)goto MSHELL_PARSE_ARG_ERROR;//有且只有一个'-'
					switch(ch)
					{
						case 'x':case 'X':token.no = 16;continue;//16进制
						case 'b':case 'B':token.no = 2;continue; //2进制
						case 'O':case 'o':token.no = 8;continue; //8进制
						default:goto MSHELL_PARSE_ARG_ERROR;     //其它进制
					}
				}
				//本参数结束
				if(ch==')' || ch==',')
				{
					token.arg[token.line].value=(void*)(token.value*token.sign);//保存数值
					token.line++;                                               //下一个
					if(ch==','){token.step=2;continue;}                         //继续下一个参数 
					return token.line;                                          //')'结尾，表示结束,返回参数个数
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
				if(ch=='\\'){token.step=4;continue;}                              //转义开始
				if(ch=='"'){                                                      //字符串结束
					token.arg[token.line].value =(void*)token.arg[token.line].data; //保存字符串地址
					token.offset=0;                                                 //清0偏移
					token.line++;                                                   //下一个
					token.data=(char*)token.arg[token.line].data;                   //char*缓冲
					token.step=2;                                                   //继续下一个参数的读入
					continue;
				}
				else
					token.data[token.offset++]=ch;                                  //保存当前字符
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
					case 'v':ch='\v';break;case 'f':ch='\f';break;case 'r':ch='\r';break;case 'e':ch=27  ;break;
					case '0':ch='\0';break;case '"':ch='"' ;break;case '\\':ch='\\';break;case '?':ch='\?';break;
					case '\'':ch='\'';break;case 'x':{token.no=token.value=0;token.step=5;continue;}
					default:
						if(is_8hex(ch))goto MSHELL_PARSE_8;//8进制 xxx
						goto MSHELL_PARSE_ARG_ERROR;
				}
				token.data[token.offset++]=ch;//保存当前转换后的数值
				token.step=3;continue;
			}
			case 5://16进制转码
			{
				if(!is_hex(ch))goto MSHELL_PARSE_ARG_ERROR;
				token.value<<=4;token.value+=hex_bin(ch);//计算数值
				token.no++;
				if(token.no==2)//xNN,只有两个16进制码
				{
					token.data[token.offset++]=(char)token.value;//保存当前值
					token.no=token.value=0;
					token.step=3;//继续字符串的接收
				}
				continue;
			}
			MSHELL_PARSE_8:
			token.no=token.value=0;token.step=6;//缓存清0，跳到6
			case 6://8进制转码
			{
				if(!is_8hex(ch))goto MSHELL_PARSE_ARG_ERROR;
				token.value<<=3;token.value+=ch-'0';//计算数值
				token.no++;
				if(token.no==3)//\xxx 8进制只有3个字符
				{
					token.data[token.offset++]=(char)token.value;
					token.no=token.value=0;
					token.step=3;//继续字符串的接收
				}
				continue;
			}
			default:goto MSHELL_PARSE_ARG_ERROR;
		}
  }
	return token.line;
	MSHELL_PARSE_ARG_ERROR://参数解析错误
	memset(&token,0,sizeof(token));
	return -1;
}

/******************************************************************************
// 查找命令
* dir :路径
* name:命令字符串
* g   :命令权限
* return:命令信息
* note:
   1.支持目录时   ,命令以 目录为标识，集中放在一个目录下
   2.不支持目录时 ,命令统一放在一起
******************************************************************************/
static Mshell_Cmd*mshell_cmd_get(char*dir,char*name,int g)
{
#if MSHELL_USING_DIR>0
	extern const int FSymDir$$Base;
	extern const int FSymDir$$Limit;
	Mshell_Dir*dirs = (Mshell_Dir*)&FSymDir$$Base;
	Mshell_Dir*dire = (Mshell_Dir*)&FSymDir$$Limit;
	/*--------------当前路径命令--------------------------------*/
	while(dirs != dire)
	{
		if( (strcmp( dir, dirs->dir ) == 0) && (g>=dirs->grade) )//是否当前目录下,等级是否足够
		{
			Mshell_Cmd*cs = dirs->start;//该目录下命令集合开始
			Mshell_Cmd*ce = dirs->end;  //该目录下命令集合结束
			/*遍历该目录下所有命令*/
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
	/*--------------遍历所有命令查找------------------------------*/
	Mshell_Cmd*cs = (Mshell_Cmd*)&MSHELL$$Base;  //命令集合开始
	Mshell_Cmd*ce = (Mshell_Cmd*)&MSHELL$$Limit; //命令集合结束
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
		cs++;
	}
	return NULL;
#endif
}

/******************************************************************************
// 调用函数
* line :命令行
******************************************************************************/
void mshell_cmd_call(char*line)
{
	char*out=NULL;                 //命令的最终路径
	char*p=NULL;                   //命令名称字符串
	char*n=strchr(line,'(');       //参数开始的地方
	char name[MSHELL_CMD_SIZE]={0};//实际命令名称
	int a = 0;                     //参数个数
	Mshell_Cmd*cmd;                //命令信息
		
	/*-------------解析当前路径----------------------------------------------*/
#if MSHELL_USING_DIR>0
	char dir[MSHELL_DIR_MAX] = {0};
	extern char* dir_search_before(char*line,char s,char e);
	p=dir_search_before(line,'/','(');//查找'('前的'/'
	if( (p!=NULL && ((n==NULL)||((n!=NULL)&&(p<n))))) //'/'必须在'('之前
	{
		char indir[MSHELL_DIR_MAX] = {0};
	  memcpy(indir,line,p-line); //indir 是输入的路径
		p++;                       //前面的是路径,后面的是名称
		out = dir_parse(mshell_dir,indir,dir);//合成当前路径,dir存放当前路径
		if(out == NULL){
			mshell_printf("dir inexist.");
			return;
		}
	}else{
		p = line;                    //输入路径为空，使用当前路径
		out = dir_parse(mshell_dir,"./",dir);//使用当前路径
	}
#else
	p=line;//不支持路径时，line就是命令
#endif
	
	/*-------------查找命令----------------------------------------------*/
	memcpy(name,p,(n==NULL)?strlen(p):(n-p));           //copy name
  cmd = mshell_cmd_get(out,name,mshell_grade());      //out为命令路径
	if(cmd==NULL){mshell_printf("No such cmd.");return;}//命令为空
	
	/*-------------解析参数----------------------------------------------*/
	a = mshell_input_arg_parse(n);//返回参数个数
	/*-------------调用命令----------------------------------------------*/
	switch(a)
	{
			case 0:a= cmd->func();break;
			case 1:a= cmd->func(ARG(0));break;
			case 2:a= cmd->func(ARG(0),ARG(1));break;
			case 3:a= cmd->func(ARG(0),ARG(1),ARG(2));break;
			case 4:a= cmd->func(ARG(0),ARG(1),ARG(2),ARG(3));break;
			case 5:a= cmd->func(ARG(0),ARG(1),ARG(2),ARG(3),ARG(4));break;
			case 6:a= cmd->func(ARG(0),ARG(1),ARG(2),ARG(3),ARG(4),ARG(5));break;
			default:
				if(a==-1)mshell_printf("args error");
			  return;
	}
	mshell_printf("\n\t%d,%08x",a,a);//输出命令执行完成后的结果
	return;
}


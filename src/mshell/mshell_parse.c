
/******************************************************************************
	mshell_parse.c 
	����: ����������
	1.֧��·��
	2.֧��Ȩ��
	3.֧�����ֲ���: �ַ��� ��ֵ(2����0b 10���� 8����0O 16����0x)
******************************************************************************/
#include "mshell.h"
#include "string.h"


extern int mshell_grade(void);

#if MSHELL_USING_DIR>0
extern char mshell_dir[];
extern char*dir_parse(char*sdir,char*dir,char*out);
#endif
/******************************************************************************
		���ͺ�
******************************************************************************/
typedef enum{
	ARG_TYPE_STRING =0,
	ARG_TYPE_NUMBER =1,
}ARG_TYPE;
#define ARG(o)  ((void*)token.arg[o].value)
	
/*�ж���ֵ,hex��,8������*/
#define is_number(ch)    (ch>='0'&&ch<='9')
#define is_hex(ch)       (is_number(ch)||(ch>='a'&&ch<='f')||(ch>='A'&&ch<='F'))
#define is_8hex(ch)      (ch>='0'&&ch<='7')
/******************************************************************************
		˽��
******************************************************************************/
/*���hex���Ӧ��ֵ
* h    :������ַ�
*return:��Ӧ��hexֵ
*/
static char hex_bin(char h)
{
	if(h>='0'&&h<='9')return h-'0';
	if(h>='a'&&h<='f')return h-'a'+10;
	if(h>='A'&&h<='F')return h-'A'+10;
	return 0;
}

/* ת���ַ�ch
* ch    :������ַ�
* jinzhi:��Ӧ�Ľ���,֧�� 2,8,10,16
*return :��Ӧ��ֵ,  <0:����
*/
int is_jinzhi(char ch,char jinzhi)
{
	switch(jinzhi)
	{
		case 2:if(ch!='0'&& ch!='1')return -1;else return ch-'0';//2����
		case 8:if(ch <'0'&& ch >'7')return -1;else return ch-'0';//8����
		case 16:if(!is_hex(ch))return 1;else return hex_bin(ch);  //16����
		case 10:                                           //10����
			     if(!is_number(ch))return -1;
					 else return ch-'0';
	}
	return 0;
}
	

/******************************************************************************
// ���Ͳ���
// ֻ֧�ּ��ֲ���
  1.""   �ַ���
	2.long ����
	3.0x   Hex�� ==>ת�� ����
******************************************************************************/
typedef struct{void*value;long data[MSHELL_CMD_ARG_LEN/sizeof(long)];}Arg;//type=1 �ַ���  2��ֵ
typedef struct{
	char*data; S8 line,offset,step;
	//============��ֵת��ʱ��=======//
	S8 sign;//����λ
	S8 no;  //�ڼ���
	long value;//�������ݼ��㻺��
	//============ת����Ĳ���=======//
	Arg arg[MSHELL_CMD_ARG_MAX];
}Token;
static Token token={NULL,0,0,0};
/*
* ����һ���ַ�
* ch     :������ַ�
* return :0 δ���  >0��������  <0:����ʧ��
*/
static int mshell_input_arg_parse(char*line)
{
	char ch;
	memset(&token,0,sizeof(token));
	token.data=(char*)token.arg[0].data;
	if(line==NULL)return 0;
	while( ((ch=*line++)!='\0') && (token.line<MSHELL_CMD_ARG_MAX) )
	{
		/*������ʽ("xxxxxx(,",12345, 5678,"123213",0x123)*/
		switch(token.step)
		{
			/*-----------�ȴ�'('------------*/
			case 0:if(ch=='('){token.step++;continue;}
			case 1:
				if(ch ==' ')continue;
				token.step++;	
			/*-----------������ʼ-----------*/
			case 2:
				if(ch==')')return token.line;                         //��������
				if(ch==','&&token.line==0)goto MSHELL_PARSE_ARG_ERROR;//��һ��֮ǰ����������','
				if(ch==',' || ch==' ')continue;                       //','' '������
				if(ch=='"'){token.offset=0; token.step=3;continue;}	  //�ַ�����ʼ	
				if(!is_number(ch)&&ch!='-')                           //�����֣��Ƿ��ţ���������
					goto MSHELL_PARSE_ARG_ERROR;
				token.step=10;//��������ת��
				token.value=0;//����ֵ��0
				token.sign=1; //Ĭ�Ϸ���+
				token.no=10;  //Ĭ��10����
			/*-----------����ת��-----------*/
			//16:0x  8:0o 2:0b
			case 10:
			{
				if(ch==' ')continue;
				if(ch=='-'){token.sign=-1;continue;}
				token.offset++;
				if(ch=='0'&& token.offset==1){token.no=11; continue;}
				if(token.no==11)
				{
					//if(token.sign==-1)goto MSHELL_PARSE_ARG_ERROR;//����ֻ��һ��'-'
					switch(ch)
					{
						case 'x':case 'X':token.no = 16;continue;//16����
						case 'b':case 'B':token.no = 2;continue; //2����
						case 'O':case 'o':token.no = 8;continue; //8����
						default:goto MSHELL_PARSE_ARG_ERROR;     //��������
					}
				}
				//����������
				if(ch==')' || ch==',')
				{
					token.arg[token.line].value=(void*)(token.value*token.sign);//������ֵ
					token.line++;                                               //��һ��
					if(ch==','){token.step=2;continue;}                         //������һ������ 
					return token.line;                                          //')'��β����ʾ����,���ز�������
				}
				//�����Ƿ��ʽ��
				{
					int result = is_jinzhi(ch,token.no);                    
					if( result <0)
					goto MSHELL_PARSE_ARG_ERROR;
					ch = (char)result;
			  }
				//������ǰֵ
				token.value*=token.no;
				token.value+=ch;
			}
			continue;	
			/*-----------�ַ���-------------*/
			case 3:
			{
				if(ch=='\\'){token.step=4;continue;}                              //ת�忪ʼ
				if(ch=='"'){                                                      //�ַ�������
					token.arg[token.line].value =(void*)token.arg[token.line].data; //�����ַ�����ַ
					token.offset=0;                                                 //��0ƫ��
					token.line++;                                                   //��һ��
					token.data=(char*)token.arg[token.line].data;                   //char*����
					token.step=2;                                                   //������һ�������Ķ���
					continue;
				}
				else
					token.data[token.offset++]=ch;                                  //���浱ǰ�ַ�
				continue;
			}
			case 4:
			{		
				/* ת�� 
				        \a ����  \b �˸�  \t �Ʊ�  \n ����  \v �����Ʊ�    \f ��ҳ  \r �س�  \e esc   \0 0  \" "   \\ \   \? ?   \' '  \ddd �˽���  \xhh ʮ������
				*/
				switch(ch)
				{
					case 'a':ch='\a';break;case 'b':ch='\b';break;case 't':ch='\t';break;case 'n':ch='\n';break;
					case 'v':ch='\v';break;case 'f':ch='\f';break;case 'r':ch='\r';break;case 'e':ch=27  ;break;
					case '0':ch='\0';break;case '"':ch='"' ;break;case '\\':ch='\\';break;case '?':ch='\?';break;
					case '\'':ch='\'';break;case 'x':{token.no=token.value=0;token.step=5;continue;}
					default:
						if(is_8hex(ch))goto MSHELL_PARSE_8;//8���� xxx
						goto MSHELL_PARSE_ARG_ERROR;
				}
				token.data[token.offset++]=ch;//���浱ǰת�������ֵ
				token.step=3;continue;
			}
			case 5://16����ת��
			{
				if(!is_hex(ch))goto MSHELL_PARSE_ARG_ERROR;
				token.value<<=4;token.value+=hex_bin(ch);//������ֵ
				token.no++;
				if(token.no==2)//xNN,ֻ������16������
				{
					token.data[token.offset++]=(char)token.value;//���浱ǰֵ
					token.no=token.value=0;
					token.step=3;//�����ַ����Ľ���
				}
				continue;
			}
			MSHELL_PARSE_8:
			token.no=token.value=0;token.step=6;//������0������6
			case 6://8����ת��
			{
				if(!is_8hex(ch))goto MSHELL_PARSE_ARG_ERROR;
				token.value<<=3;token.value+=ch-'0';//������ֵ
				token.no++;
				if(token.no==3)//\xxx 8����ֻ��3���ַ�
				{
					token.data[token.offset++]=(char)token.value;
					token.no=token.value=0;
					token.step=3;//�����ַ����Ľ���
				}
				continue;
			}
			default:goto MSHELL_PARSE_ARG_ERROR;
		}
  }
	return token.line;
	MSHELL_PARSE_ARG_ERROR://������������
	memset(&token,0,sizeof(token));
	return -1;
}

/******************************************************************************
// ��������
* dir :·��
* name:�����ַ���
* g   :����Ȩ��
* return:������Ϣ
* note:
   1.֧��Ŀ¼ʱ   ,������ Ŀ¼Ϊ��ʶ�����з���һ��Ŀ¼��
   2.��֧��Ŀ¼ʱ ,����ͳһ����һ��
******************************************************************************/
static Mshell_Cmd*mshell_cmd_get(char*dir,char*name,int g)
{
#if MSHELL_USING_DIR>0
	extern const int FSymDir$$Base;
	extern const int FSymDir$$Limit;
	Mshell_Dir*dirs = (Mshell_Dir*)&FSymDir$$Base;
	Mshell_Dir*dire = (Mshell_Dir*)&FSymDir$$Limit;
	/*--------------��ǰ·������--------------------------------*/
	while(dirs != dire)
	{
		if( (strcmp( dir, dirs->dir ) == 0) && (g>=dirs->grade) )//�Ƿ�ǰĿ¼��,�ȼ��Ƿ��㹻
		{
			Mshell_Cmd*cs = dirs->start;//��Ŀ¼������Ͽ�ʼ
			Mshell_Cmd*ce = dirs->end;  //��Ŀ¼������Ͻ���
			/*������Ŀ¼����������*/
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
	/*--------------���������������------------------------------*/
	Mshell_Cmd*cs = (Mshell_Cmd*)&MSHELL$$Base;  //����Ͽ�ʼ
	Mshell_Cmd*ce = (Mshell_Cmd*)&MSHELL$$Limit; //����Ͻ���
  while(cs != ce)
	{
		if(strcmp(name,cs->name)==0)
		{
			if(g>=cs->grade)
			{
				return cs;  //��������
			}else{
				return NULL;//��Ȩ����
			}
		}
		cs++;
	}
	return NULL;
#endif
}

/******************************************************************************
// ���ú���
* line :������
******************************************************************************/
void mshell_cmd_call(char*line)
{
	char*out=NULL;                 //���������·��
	char*p=NULL;                   //���������ַ���
	char*n=strchr(line,'(');       //������ʼ�ĵط�
	char name[MSHELL_CMD_SIZE]={0};//ʵ����������
	int a = 0;                     //��������
	Mshell_Cmd*cmd;                //������Ϣ
		
	/*-------------������ǰ·��----------------------------------------------*/
#if MSHELL_USING_DIR>0
	char dir[MSHELL_DIR_MAX] = {0};
	extern char* dir_search_before(char*line,char s,char e);
	p=dir_search_before(line,'/','(');//����'('ǰ��'/'
	if( (p!=NULL && ((n==NULL)||((n!=NULL)&&(p<n))))) //'/'������'('֮ǰ
	{
		char indir[MSHELL_DIR_MAX] = {0};
	  memcpy(indir,line,p-line); //indir �������·��
		p++;                       //ǰ�����·��,�����������
		out = dir_parse(mshell_dir,indir,dir);//�ϳɵ�ǰ·��,dir��ŵ�ǰ·��
		if(out == NULL){
			mshell_printf("dir inexist.");
			return;
		}
	}else{
		p = line;                    //����·��Ϊ�գ�ʹ�õ�ǰ·��
		out = dir_parse(mshell_dir,"./",dir);//ʹ�õ�ǰ·��
	}
#else
	p=line;//��֧��·��ʱ��line��������
#endif
	
	/*-------------��������----------------------------------------------*/
	memcpy(name,p,(n==NULL)?strlen(p):(n-p));           //copy name
  cmd = mshell_cmd_get(out,name,mshell_grade());      //outΪ����·��
	if(cmd==NULL){mshell_printf("No such cmd.");return;}//����Ϊ��
	
	/*-------------��������----------------------------------------------*/
	a = mshell_input_arg_parse(n);//���ز�������
	/*-------------��������----------------------------------------------*/
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
	mshell_printf("\n\t%d,%08x",a,a);//�������ִ����ɺ�Ľ��
	return;
}


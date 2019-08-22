#ifndef __M_SHELL_H__
#define __M_SHELL_H__

#include "mshell_config.h"

/******************************************************************************
		����
******************************************************************************/
//�ϲ��������� 
#define MSHXY(x,y) x##y
#define MSHCXY(x,y) MSHXY(x,y)

//�ϲ���������
#define MSHXYLINE(class,name)    class##_##name##_line

//����������(������)
#define MSH_UnknownName(class,name)       MSHCXY(MSHXYLINE(class,name),__LINE__)
	
/*���ض����õ��ĺ�*/
#define msh_used          						__attribute__((used))				/*used����ʾ���Ż�*/
#define mshsection(x)                 __attribute__((section(x))) /*���ض���*/

/******************************************************************************
******************************************************************************/
/*�������,ʵ����Ϊ long(*fn)(...),���Ϊʡ�Բ�����Ĭ��ΪNULL*/
typedef int (*syscall_func)();

/*��������*/
typedef struct{
	
	#if MSHELL_USING_DIR>0
  char*dir;               //·��֧��
	#else
	int grade;              //����Ȩ�޵ȼ�
	#endif
	
	char*name;              //����	
	char*desc;              //������Ϣ
	syscall_func func;      //�����
}Mshell_Cmd;


/******************************************************************************
// ���������
******************************************************************************/
/*dir:·��  cmd:�������ʱ����  f:����� info:����˵��*/
#if MSHELL_USING_DIR>0
#define Export(dir,cmd,f,i) Mshell_Cmd const MshellCmd_##cmd msh_used mshsection(#dir) = \
       {(char*)#dir,(char*)#cmd,(char*)i,(syscall_func)&f};
#else 
#define Export(grade,cmd,f,i) Mshell_Cmd const MshellCmd_##cmd __attribute__((section("MSHELL")))  __attribute__((used)) = \
       {grade,(char*)#cmd,(char*)i,(syscall_func)&f};			 
#endif

/******************************************************************************
// Ŀ¼֧��
******************************************************************************/
#if MSHELL_USING_DIR>0

/*·������*/
typedef struct{
	const char* dir;   /*Ŀ¼*/
	const int   grade; /*Ŀ¼Ȩ�޵ȼ�������¼�ȼ�>=Ŀ¼�ȼ����ſ��Է���Ŀ¼������*/
	
	/*�����*/
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
// ȫ�ֺ���
******************************************************************************/			 
void mshell(char data);

#endif


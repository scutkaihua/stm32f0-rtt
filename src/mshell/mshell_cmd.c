
#include "mshell.h"
#include "string.h"
#include "stdio.h"
/*************************************************************************
*			ͨ������
**************************************************************************/
extern char mshell_dir[];
extern long list_dir(int g,char*sh_dir,char*indir,char*dir);
extern char*dir_parse(char*sdir,char*dir,char*out);

extern int mshell_grade(void);

Export_DIR("/",root,0);

/*�б�ǰĿ¼���б����б���*/
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
	mshell_printf("-- ����------------ ����------------------");
	while(cs != ce)
	{
		if(mshell_grade()>=cs->grade)
		 mshell_printf("\n%-16s -- %s", cs->name, cs->desc);
		 cs++;
	}
	#endif
}

/*�汾*/
static long ver(void)
{
	mshell_printf(MSHELL_VERSION);
	return 1;
}

/*����*/
static long cls(void)
{
	mshell_printf("\033[2J");
	return 1;
}
extern long cd(char*in_dir);
Export(root,ls,list,        	  "ls ���� ls() �б�����")
Export(root,ver,ver,        	  "ver ���� ver() �汾��")
Export(root,cls,cls,        	  "cls ���� cls() ����")
#if MSHELL_USING_DIR>0
Export(root,cd,cd,							"cd string  ���� cd(string)")
#endif

#if MSHELL_USING_LOGIN > 0
extern int login(char*line);
extern int logout(void);
Export(root,login,login,        "login xxx xxxx ��¼")
Export(root,logout,logout,      "logout �ǳ�")
#endif

/*finsh�������У���������*/
int finsh_run_line_first(char*line)
{
	if(strlen(line)==0)return 1;
	mshell_printf("\n");
  /*================cd===============================*/
	#if MSHELL_USING_DIR>0
	if( memcmp("cd ",line,3)==0 )                      //�в�cd
	{
		char dir[MSHELL_DIR_MAX] = {0};
		sscanf((const char*)line,"cd %s",dir);//��ȡ����
		if(strlen(dir)>0){dir[strlen(dir)]=0;}
		cd(dir);//����Ŀ¼
		return 1;
	}else if( memcmp("cd",line,2)==0 ){cd(0); return 1;}//�޲�cd
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


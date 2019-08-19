
#include "mshell.h"
#include "string.h"
#include "string.h"
/*************************************************************************
*			ͨ������
**************************************************************************/
extern char mshell_dir[];
extern long list_dir(int g,char*sh_dir,char*indir,char*dir);
extern int finsh_grade;
extern char*dir_parse(char*sdir,char*dir,char*out);

Export_DIR("/",root,0);
static long cd(char*in_dir)
{
	char*in = in_dir;
	char dir[MSHELL_DIR_MAX] = {0};
	char*out = NULL;
	out = dir_parse(mshell_dir,in_dir,dir);
  if(dir[0]==0){memset(dir,0,sizeof(dir));dir[0]='\/';}
	if(out != NULL)
	{	
		if( is_dir(dir) != NULL)
		{
			memset(mshell_dir,0,MSHELL_DIR_MAX);
			strcpy(mshell_dir,dir);
			mshell_printf("Dir :%s",dir);
			mshell_dir[strlen(mshell_dir)] = '>';
			return 1;
		}else{
			mshell_printf("No Dir:%s",dir);
		}
	}else{
		mshell_printf("\nDirectory Error.");
	}
	return 0;
}

/*�б�ǰĿ¼���б����б���*/
static long list(char*indir)
{
	extern const int FSymDir$$Base;
	extern const int FSymDir$$Limit;
	Mshell_Cmd*dirs = (Mshell_Cmd*)&FSymDir$$Base;
	Mshell_Cmd*dire = (Mshell_Cmd*)&FSymDir$$Limit;
  char dir[MSHELL_DIR_MAX] = {0};
	return list_dir(finsh_grade,mshell_dir,indir,dir);
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

Export(root,ls,list,        	  "ls ���� ls() �б�����")
Export(root,cd,cd,							"cd string  ���� cd(string)")
Export(root,ver,ver,        	  "ver ���� ver() �汾��")
Export(root,cls,cls,        	  "cls ���� cls() ����")

/*finsh�������У���������*/
int finsh_run_line_first(char*line)
{
	char*str = "cd ";
	int size = strlen(str);
	
	mshell_printf("\n");
	if( memcmp(str,line,size)==0 )
	{
		char dir[MSHELL_DIR_MAX] = {0};
		sscanf(line,"cd %s",dir);
		if(strlen(dir)>0){dir[strlen(dir)]=0;}
		cd(dir);
		return 1;
	}
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
	if((strcmp("ver",line)==0)||(strcmp("ver()",line)==0))
	{
		ver();return 1;
	}
	if((strcmp("cls",line)==0)||(strcmp("cls()",line)==0))
	{
		cls();return 1;
	}
	
	return 0;
}
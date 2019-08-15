#include "string.h"
#include "mshell.h"


#if MSHELL_USING_DIR>0

/*��ǰĿ¼*/
char mshell_dir[MSHELL_DIR_MAX] = {'\/',0};

/******************************************************************************
// 
******************************************************************************/
/*���� cd ���� ���ؽ�������ַ���*/
char*dir_parse(char*sdir,char*dir,char*out)
{
	if( (dir==NULL)||(sdir==NULL) ||(out==NULL))return NULL;
	{
		int size = strlen(dir);
		memcpy(out,sdir,strlen(sdir));
		if(out[strlen(out)-1]== '>')out[strlen(out)-1]=0;
		if(out[strlen(out)-1]!= '\/'){out[strlen(out)]='\/';}

		if(dir[0]=='\/'){
			
			//��ȡǰ��Ŀ¼
			char*p = strchr(dir+1,'\/');
			if(p==0){
				memcpy(out,dir,strlen(dir)); 
				out[strlen(dir)]=0;
			  return out;
			}
			else{
				//������������
				memcpy(out,dir,(int)p-(int)dir);
				out[(int)p-(int)dir]=0;
				dir = p++;
				size = strlen(dir);
			}
		}
		
		while(size>0){
			//����һ���ֽ�dir
			switch(*dir)
			{
				case '.':
					if(dir[1] == '.')
					{
						char *p =NULL;
						//��һ��
						if(out[strlen(out)-1] == '\/')out[strlen(out)-1] = 0;
						if((p = strrchr(out,'\/'))!=0){
							dir+=2;size-=2;
							p[0] = 0;
							continue;
						}else{
							mshell_printf("No Dir:%s%s\r\n",out,"..");
							return NULL;
						}
					}else{
						dir++;
						size--;
					}
					break;
					
				case '\/':
					if( out[strlen(out)-1] == '\/'){
						dir++;
						size--;
						break;
					}
				default:
					out[strlen(out)+1] = 0;out[strlen(out)]=*dir;
				  dir++;
				  size--;
				  break;
			}
		}
		
		if( out[strlen(out)-1] == '\/')out[strlen(out)-1] = 0;
		if( out[0]==0 ){out[0]='/';out[1]=0;}
		return out;
	}
}

/******************************************************************************
// ����: �б�Ŀ¼
******************************************************************************/
long list_dir(int g,char*sh_dir,char*indir,char*dir)
{
	extern int finsh_grade;
	extern const int FSymDir$$Base;
	extern const int FSymDir$$Limit;
	Mshell_Dir*dirs = (Mshell_Dir*)&FSymDir$$Base;
	Mshell_Dir*dire = (Mshell_Dir*)&FSymDir$$Limit;
	memcpy(dir,sh_dir,strlen(sh_dir));
	dir[strlen(dir)-1] = 0;
	
	if( indir != NULL )
	{
		//����Ŀ¼ 
		char* p = dir_parse(dir,indir,dir);
		if(p == NULL)return -1;
	}
  if(dir[0]==0){dir[0]='\/';dir[1] = 0;}
	mshell_printf("Dir :%s\r\n",dir);
	mshell_printf("-- ����----------- ����------------------\n");
//	mshell_printf("--Global List:\n");
	
	//��ǰ·������
	while(dirs != dire)
	{
		if( (strcmp( dir, dirs->dir ) == 0) && (g>=dirs->grade) )
		{
			Mshell_Cmd*cs = dirs->start;
			Mshell_Cmd*ce = dirs->end;
			while(cs != ce){
				mshell_printf("%-16s -- %s\n", cs->name, cs->desc);
				cs++;
			}
		}else{
			//Ŀ¼
			if( (memcmp(dirs->dir,dir,strlen(dir))==0) && (g>=dirs->grade) ){
				char*pdir = (char*)(&dirs->dir[strlen(dir)]);
				if( pdir[0] == '\/')pdir++;
				if( strchr(pdir,'\/')==0)
					{
						mshell_printf("%-16s -- %s\n",pdir, "Ŀ¼");
					}
			}
		}
		
		dirs++;
	}
	return 0;
}
/******************************************************************************
// ����: ����Ŀ¼
******************************************************************************/
/*�ж�Ŀ¼�Ƿ����*/
Mshell_Dir*is_dir(char*dir){
	extern const int FSymDir$$Base;
	extern const int FSymDir$$Limit;
	Mshell_Dir*dirs = (Mshell_Dir*)&FSymDir$$Base;
	Mshell_Dir*dire = (Mshell_Dir*)&FSymDir$$Limit;

	//��ǰ·������
	while(dirs != dire){
		if(strcmp( dir, dirs->dir ) == 0)return dirs;
		dirs++;
	}	
	return NULL;
}
static long cd(char*in_dir)
{
	char*in = in_dir;
	char dir[MSHELL_DIR_MAX] = {0};
	char*out = dir_parse(mshell_dir,in_dir,dir);
  if(dir[0]==0){memset(dir,0,sizeof(dir));dir[0]='\/';}
	if(out != NULL)
	{	
		if( is_dir(dir) != NULL)
		{
			memset(mshell_dir,0,sizeof(mshell_dir));
			strcpy(mshell_dir,dir);
			mshell_printf("Dir :%s\r\n",dir);
			mshell_dir[strlen(mshell_dir)] = '>';
		}else{
			mshell_printf("No Dir:%s\r\n",dir);
		}
	}else{
		mshell_printf("Directory Error.\r\n");
	}
	return 0;
}

#endif


/*************************************************************************
*			mshell_dir.c
*  目录处理
**************************************************************************/

#include "string.h"
#include "mshell.h"

extern int mshell_grade(void);

#if MSHELL_USING_DIR>0

/*当前目录*/
char mshell_dir[MSHELL_DIR_MAX] = {'/',0};
/******************************************************************************
* 查找 '(' 之前的 '/'
* line   :输入字符串
* s      :要搜索的字符
* e      :结尾字符
******************************************************************************/
char* dir_search_before(char*line,char s,char e)
{
	char*get=NULL;
	while(*line!=0)
	{
		if(*line==s)get= line;
		else if(*line==e)return get;
		line++;
	}
	return get;
}
/******************************************************************************
*  更改目录
*  sdir :当前目录
*  dir  :要运算的路径
*  out  :运算后的路径
* return:返回运算后的最终路径
******************************************************************************/
/*解释 cd 命令 返回解析后的字符串*/
char*dir_parse(char*sdir,char*dir,char*out)
{
	if( (dir==NULL)||(sdir==NULL) ||(out==NULL))return NULL;
	{
		int size = strlen(dir);
		memcpy(out,sdir,strlen(sdir));
		if(out[strlen(out)-1]== '>')out[strlen(out)-1]=0;
		if(out[strlen(out)-1]!= '/'){out[strlen(out)]='/';}

		if(dir[0]=='/'){
			
			//截取前面目录
			char*p = strchr(dir+1,'/');
			if(p==0){
				memcpy(out,dir,strlen(dir)); 
				out[strlen(dir)]=0;
			  return out;
			}
			else{
				//解析后面内容
				memcpy(out,dir,(int)p-(int)dir);
				out[(int)p-(int)dir]=0;
				dir = p++;
				size = strlen(dir);
			}
		}
		
		while(size>0){
			//处理一个字节dir
			switch(*dir)
			{
				case '.':
					if(dir[1] == '.')
					{
						char *p =NULL;
						//上一级
						if(out[strlen(out)-1] == '/')out[strlen(out)-1] = 0;
						if((p = strrchr(out,'/'))!=0){
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
					
				case '/':
					if( out[strlen(out)-1] == '/'){
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
		
		if( out[strlen(out)-1] == '/')out[strlen(out)-1] = 0;
		if( out[0]==0 ){out[0]='/';out[1]=0;}
		return out;
	}
}

/******************************************************************************
*  列表目录下的内容
*  g     :当前权限等级
*  sh_dir:当前目录 
*  indir :要运算的路径
*  dir   :运算后的路径,所有命令在这个路径下找
* return :>=0:OK   <0:失败
******************************************************************************/
int list_dir(int g,char*sh_dir,char*indir,char*dir)
{
	extern const int FSymDir$$Base;
	extern const int FSymDir$$Limit;
	Mshell_Dir*dirs = (Mshell_Dir*)&FSymDir$$Base;
	Mshell_Dir*dire = (Mshell_Dir*)&FSymDir$$Limit;
	memcpy(dir,sh_dir,strlen(sh_dir));
	dir[strlen(dir)-1] = 0;
	
	if( indir != NULL )
	{
		//进入目录 
		char* p = dir_parse(dir,indir,dir);
		if(p == NULL)return -1;
	}
  if(dir[0]==0){dir[0]='/';dir[1] = 0;}
	mshell_printf("Dir :%s\n",dir);
	mshell_printf("-- 命令------------ 描述------------------");
//	mshell_printf("--Global List:\n");
	
	//当前路径命令
	while(dirs != dire)
	{
		if( (strcmp( dir, dirs->dir ) == 0) && (g>=dirs->grade) )
		{
			Mshell_Cmd*cs = dirs->start;
			Mshell_Cmd*ce = dirs->end;
			while(cs != ce){
				mshell_printf("\n%-16s -- %s", cs->name, cs->desc);
				cs++;
			}
		}else{
			//目录
			if( (memcmp(dirs->dir,dir,strlen(dir))==0) && (g>=dirs->grade) ){
				char*pdir = (char*)(&dirs->dir[strlen(dir)]);
				if( pdir[0] == '/')pdir++;
				if( strchr(pdir,'/')==0)
					{
						mshell_printf("\n%-16s -- %s",pdir, "目录");
					}
			}
		}
		
		dirs++;
	}
	return 0;
}
/******************************************************************************
// 命令: 进入目录
* in_dir:要进入的目录
******************************************************************************/
/*判断目录是否存在*/
Mshell_Dir*is_dir(char*dir){
	extern const int FSymDir$$Base;
	extern const int FSymDir$$Limit;
	Mshell_Dir*dirs = (Mshell_Dir*)&FSymDir$$Base;
	Mshell_Dir*dire = (Mshell_Dir*)&FSymDir$$Limit;

	//当前路径命令
	while(dirs != dire){
		if(strcmp( dir, dirs->dir ) == 0)return dirs;
		dirs++;
	}	
	return NULL;
}
int cd(char*in_dir)
{
	char dir[MSHELL_DIR_MAX] = {0};
	char*out = dir_parse(mshell_dir,(in_dir==NULL)?("./"):(in_dir),dir);
  if(dir[0]==0){memset(dir,0,sizeof(dir));dir[0]='/';}
	if(out != NULL)
	{	
		if( is_dir(dir) != NULL)
		{
			memset(mshell_dir,0,sizeof(mshell_dir));
			strcpy(mshell_dir,dir);
			mshell_printf("Dir :%s",dir);
			mshell_dir[strlen(mshell_dir)] = '>';
		}else{
			mshell_printf("No Dir:%s",dir);
		}
	}else{
		mshell_printf("Directory Error.");
	}
	return 0;
}

#endif


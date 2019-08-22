#include "mshell.h"
#include "string.h"
#include "stdio.h"
#if MSHELL_USING_LOGIN > 0
int logout(void);
int login(char*line);
extern char mshell_dir[];

/******************************************************************************
*    权限配置(用户名，密码，等级)
******************************************************************************/
typedef struct{char*name;char*pw;int grade;}LOGIN;
const LOGIN logins[]={
	{"user","user",0},   //用户最低等级
	{"debug","debug",10},//调试级别
	{"root", "root",255},//超级管理员级别
};
LOGIN current_login={NULL,NULL,0};
char* login_user(void) {return current_login.name;}
int   login_grade(void){return current_login.grade;}
/******************************************************************************
*   超时退出
*  reset   :TRUE:登录开始计时  FALSE:只刷新  
*  返回    :TRUE:超时 FALSE:不超时
******************************************************************************/
extern time_t time(time_t*);
static BOOL login_timeout(BOOL reset)
{
	static time_t now =0;
	if(reset)now=time(0);
	else{
		//判断超时
		if( ((now/1000)+MSHELL_LOGIN_TIMEOUT)< (time(0)/1000) )
		{
			now=time(0);//刷新当前时间
			return TRUE;
		}
		else{
			now =time(0);//刷新当前时间
			return FALSE;
		} 
	}
	return FALSE;
}
/*按键输入时，刷新登录时间，看一下是否登录超时,如果超时，就登出*/
void login_input_pull(void)
{
	if(current_login.name==NULL)current_login=logins[0];//默认初始化用户为第一个用户
	if(login_timeout(FALSE)==TRUE)
	{
		if(memcmp(&current_login,logins,sizeof(LOGIN))==0)return;
		mshell_printf("\n");
		logout();
		mshell_printf("\n");
	}
}

/******************************************************************************
*   密码输入时,应该显示'*',而不是明文
******************************************************************************/
void login_input_replace(char*line)
{
	int i=0;
	char name[MSHELL_LOGIN_NAME_MAX]={0};char pw[MSHELL_LOGIN_PW_MAX]={0};
	if(sscanf(line,"login %s %s",name,pw)==2){
		extern void mshell_prompt(void);//前导
		mshell_printf("\33[3K\n\033[1A");//清除该行
		for(;i<MSHELL_LOGIN_PW_MAX;i++)if(pw[i]!=0)pw[i]='*';
		pw[MSHELL_LOGIN_PW_MAX-1]=0;
		mshell_prompt();//前导
		mshell_printf("login %s %s",name,pw);
	}
}

/******************************************************************************
*   登录 -1:无用户名  -2:密码错误 >=0当前等级
******************************************************************************/
int login(char*line)//line=login user pw
{
	int i=0;
	char name[MSHELL_LOGIN_NAME_MAX]={0};char pw[MSHELL_LOGIN_PW_MAX]={0};
	sscanf(line,"login %s %s",name,pw);
	for(;i<sizeof(logins)/sizeof(LOGIN);i++)
	{
		/*用户名，密码匹配成功*/
		if((strcmp(name,logins[i].name)==0)){
		 if(strcmp(pw,logins[i].pw)==0)
			{
				current_login=logins[i];
				login_timeout(TRUE);
				mshell_printf("User:%s",current_login.name);
				return current_login.grade;
			}
		 else{
				mshell_printf("password error.");
				return -2;
		 } 
	 }
	}
	mshell_printf("username error.");
	return -1;
}

/******************************************************************************
*   登出
******************************************************************************/
int logout(void)
{
	current_login=logins[0];
	mshell_printf("User:%s",current_login.name);
	return current_login.grade;
}
#endif

/******************************************************************************
*   当前权限等级
******************************************************************************/
int mshell_grade(void)
{
	#if MSHELL_USING_LOGIN > 0
	return current_login.grade;
	#else
	return 0;
	#endif
}


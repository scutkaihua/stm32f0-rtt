#include "mshell.h"
#include "string.h"
#if MSHELL_USING_LOGIN > 0
int logout(void);
int login(char*line);


/*Ȩ������*/
typedef struct{char*name;char*pw;int grade;}LOGIN;
const LOGIN logins[]={
	{"user","user",0},   //�û���͵ȼ�
	{"debug","debug",10},//���Լ���
	{"root", "root",255},//��������Ա����
};
LOGIN current_login;

/*��ʱ�˳�:reset:TRUE:��¼��ʼ��ʱ  FALSE:ֻˢ��  ����:TRUE:��ʱ FALSE:����ʱ*/
extern time_t time(time_t*);
static BOOL login_timeout(BOOL reset)
{
	static time_t now =0;
	if(reset)now=time(0);
	else{
		//�жϳ�ʱ
		if( ((now/1000)+MSHELL_LOGIN_TIMEOUT)< (time(0)/1000) )
		{
			now=time(0);
			return TRUE;
		}
		else{
			now =time(0);//ˢ�µ�ǰʱ��
			return FALSE;
		} 
	}
	return FALSE;
}
/*��������ʱ��ˢ�µ�¼ʱ�䣬��һ���Ƿ��¼��ʱ,�����ʱ���͵ǳ�*/
void login_input_pull(void)
{
	if(login_timeout(FALSE)==TRUE)
	{
		if(memcmp(&current_login,logins,sizeof(LOGIN))==0)return;
		mshell_printf("\n");
		logout();
		mshell_printf("\n");
	}
}
/*��������ʱ,Ӧ����ʾ'*',����������*/
void login_input_replace(char*line)
{
	int i=0;
	char name[MSHELL_LOGIN_NAME_MAX]={0};char pw[MSHELL_LOGIN_PW_MAX]={0};
	if(sscanf(line,"login %s %s",name,pw)==2){
		mshell_printf("\33[2K\r");//�������
		for(;i<MSHELL_LOGIN_PW_MAX;i++)if(pw[i]!=0)pw[i]='*';
		pw[MSHELL_LOGIN_PW_MAX-1]=0;
		mshell_printf("%slogin %s %s",MSHELL_PROMPT,name,pw);
	}
}

/*��¼ -1:���û���  -2:������� >=0��ǰ�ȼ�*/
int login(char*line)//line=login user pw
{
	int i=0;
	char name[MSHELL_LOGIN_NAME_MAX]={0};char pw[MSHELL_LOGIN_PW_MAX]={0};
	sscanf(line,"login %s %s",name,pw);
	for(;i<sizeof(logins)/sizeof(LOGIN);i++)
	{
		/*�û���������ƥ��ɹ�*/
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

/*�ǳ�*/
int logout(void)
{
	current_login=logins[0];
	mshell_printf("User:%s",current_login.name);
	return current_login.grade;
}
#endif

/*��ǰȨ�޵ȼ�*/
int mshell_grade(void)
{
	#if MSHELL_USING_LOGIN > 0
	return current_login.grade;
	#else
	return 0;
	#endif
}


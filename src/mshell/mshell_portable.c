
#include <stdio.h>
#include <stdarg.h>
#include "includes.h"
/******************************************************************************
������ֲ:
1.copy �ļ�:
  mshell_parse.c  //�������������
	mshell_cmd.c    //��������: ls cls ver cd login logout 
	mshell_dir.c    //Ŀ¼����֧��
	mshell_permission.c//��¼,Ȩ�޵ȼ�֧��
	mshell.c        //�����ַ�������ʷ��¼֧��
	mshell_portable.c//��ֲ�ļ�
	
	mshell_config.h //���ӻ������ļ�
	types.h         //�õ��ĳ�������������:U8 S8 U16... time_t eg.
	
2.��дmshell_portable.c����

  ��ӡ����(��ο�printfԴ��)
	(1)void mshell_printf(const char(fmt,...)
	
3.���ò���
  mshell_config.h
	
4.���ʹ��mshell
  (1)������ test_cmd.c д��
	   
     Export_DIR("/test",test,2);                       //����Ŀ¼
	   Export(test,hw,helloworld,"say hello to world!"); //��������
		 
	(2)�ַ�����
	   while(1){
			ch = �ַ��豸��1�ֽ�
			mshell(ch)
		 }

5.�û�����������,���޸�mshell_permission.c�е� logins [0]:ΪĬ�ϵ����Ȩ���û�

6.���������ʹ��

  msh>ls
	msh>ls("xxx/xxx")
	msh>ls xxx/xxx
	
	msh>ver
	msh>ver()
	
	msh>cls
	msh>cls()
	
	msh>cd
	msh>cd xxxx

  msh>login user password
	msh>logout
	
	msh>xxxx/xxxx/cmd("xxxx",123)

����:
					msh>ls
					Dir :/
					-- ����------------ ����------------------
					ls               -- ls ���� ls() �б�����
					ver              -- ver ���� ver() �汾��
					cls              -- cls ���� cls() ����
					cd               -- cd string  ���� cd(string)
					login            -- login xxx xxxx ��¼
					logout           -- logout �ǳ�
					msh>login root ****
					User:root
					msh>cd test
					Dir :/test
					msh>ls
					Dir :/test
					-- ����------------ ����------------------
					hw               -- say hello to world!
					arg              -- test arg
					rst              -- reset mcu.
					info             -- Ŀ¼
					msh>arg("kaihua",123)
					arg1:kaihua,data=123
									1,00000001
					msh>cd ..
					Dir :/
					msh>./test/info/info
					this is mshell information!
									1,00000001
					msh>./test/info/info()
					this is mshell information!
									1,00000001
					msh>

******************************************************************************/



/*mshell �Ĵ�ӡ����*/
#include "rtthread.h"
void mshell_printf(const char*fmt,...)
{
    va_list args;
    int length;
    static char rt_log_buf[RT_CONSOLEBUF_SIZE];
    va_start(args, fmt);//��ȡ�����б�
    length = rt_vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);//��ʽ���ַ���
    if (length > RT_CONSOLEBUF_SIZE - 1)//�����ж�
        length = RT_CONSOLEBUF_SIZE - 1;
		//enable_485_tx();
    ld_uart_send(1,(U8*)rt_log_buf,length);
		//while(is_enable_485_rx()==FALSE);
    va_end(args);
}

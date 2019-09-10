
#include <stdio.h>
#include <stdarg.h>
#include "includes.h"
/******************************************************************************
代码移植:
1.copy 文件:
  mshell_parse.c  //参数，命令解析
	mshell_cmd.c    //常用命令: ls cls ver cd login logout 
	mshell_dir.c    //目录运算支持
	mshell_permission.c//登录,权限等级支持
	mshell.c        //输入字符处理，历史记录支持
	mshell_portable.c//移植文件
	
	mshell_config.h //可视化配置文件
	types.h         //用到的常用数据类型如:U8 S8 U16... time_t eg.
	
2.编写mshell_portable.c依赖

  打印函数(请参考printf源码)
	(1)void mshell_printf(const char(fmt,...)
	
3.配置参数
  mshell_config.h
	
4.如何使用mshell
  (1)读参数 test_cmd.c 写法
	   
     Export_DIR("/test",test,2);                       //导出目录
	   Export(test,hw,helloworld,"say hello to world!"); //导出命令
		 
	(2)字符读入
	   while(1){
			ch = 字符设备读1字节
			mshell(ch)
		 }

5.用户名密码配置,请修改mshell_permission.c中的 logins [0]:为默认的最低权限用户

6.常用命令的使用

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

例如:
					msh>ls
					Dir :/
					-- 命令------------ 描述------------------
					ls               -- ls 或者 ls() 列表内容
					ver              -- ver 或者 ver() 版本号
					cls              -- cls 或者 cls() 清屏
					cd               -- cd string  或者 cd(string)
					login            -- login xxx xxxx 登录
					logout           -- logout 登出
					msh>login root ****
					User:root
					msh>cd test
					Dir :/test
					msh>ls
					Dir :/test
					-- 命令------------ 描述------------------
					hw               -- say hello to world!
					arg              -- test arg
					rst              -- reset mcu.
					info             -- 目录
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



/*mshell 的打印函数*/
#include "rtthread.h"
void mshell_printf(const char*fmt,...)
{
    va_list args;
    int length;
    static char rt_log_buf[RT_CONSOLEBUF_SIZE];
    va_start(args, fmt);//读取参数列表
    length = rt_vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);//格式化字符串
    if (length > RT_CONSOLEBUF_SIZE - 1)//超长判断
        length = RT_CONSOLEBUF_SIZE - 1;
		//enable_485_tx();
    ld_uart_send(1,(U8*)rt_log_buf,length);
		//while(is_enable_485_rx()==FALSE);
    va_end(args);
}

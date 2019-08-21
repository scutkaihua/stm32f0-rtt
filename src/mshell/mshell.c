
#include "mshell_config.h"
#include "mshell.h"
#include "string.h"

extern int  mshell_run_line_first(char*line);
extern void mshell_cmd_call(char*line);
/******************************************************************************
// mshell :简单调试终端
//       1.只支持命令调用，不支持变量调用
//       2.支持登录
//       3.支持路径访问
******************************************************************************/

enum input_stat
{
	NOT_INITED=-1,     //未初始化
	WAIT_NORMAL,       //正常
	WAIT_SPEC_KEY,     //上下左右键等待
	WAIT_FUNC_KEY,     //上下左右功能键状态
};
static enum input_stat stat=NOT_INITED;//输入状态
static U8 line[MSHELL_CMD_SIZE]; //命令缓存
static U8 line_position;        //接收位置
static U8 line_curpos;          //光标位置
#ifdef MSHELL_USING_ECHO        //回显
static U8 echo_mode;            //echo 模式
#endif

/******************************************************************************
// 历史记录 -1:上一条   0:保存当前命令   1:下一条
// return :1:return  0 continue
******************************************************************************/
#if MSHELL_USING_HISTORY > 0
U16 current_history;
U16 history_count;
char cmd_history[MSHELL_HISTORY_LINES][MSHELL_CMD_SIZE];
static void mshell_history(signed char func)
{
	/* 保存一条 history */
	if(func == 0)
	{
    if (line_position != 0)
    {
        /* push history */
        if (history_count >= MSHELL_HISTORY_LINES)
        {
            /* 所有记录下移*/
            int index;
            for (index = 0; index < MSHELL_HISTORY_LINES - 1; index ++)
            {
                memcpy(&cmd_history[index][0],&cmd_history[index + 1][0], MSHELL_CMD_SIZE);
            }
            memset(&cmd_history[index][0], 0   , MSHELL_CMD_SIZE);
            memcpy(&cmd_history[index][0], line, line_position);
            /* it's the maximum history */
            history_count = MSHELL_HISTORY_LINES;
        }
        else
        {
            memset(&cmd_history[history_count][0], 0   , MSHELL_CMD_SIZE);
            memcpy(&cmd_history[history_count][0], line, line_position);

            /* increase count and set current history position */
            history_count ++;
        }
    }
    current_history = history_count;	
		return;
	}	
	/* 上一条 history */
	else if(func == -1)
	{                
		if (current_history > 0)current_history --;
		else
		{
				current_history = 0;
		}
	}
	/*  下一条 history */
	else if(func == 1)/* next history */
	{            
		if (current_history < history_count - 1)
				current_history ++;
		else
		{
				/* set to the end of history */
				if (history_count != 0)
						current_history = history_count - 1;
		}	
	}
	//显示命令
	memcpy(line, &cmd_history[current_history][0],MSHELL_CMD_SIZE);
	line_curpos = line_position = strlen((const char*)line);
  mshell_printf("\033[2K\r");
  mshell_printf("%s%s", MSHELL_PROMPT,line);
}
#endif

/******************************************************************************
	初始化
******************************************************************************/
static void mshell_init(void)
{
		stat=WAIT_NORMAL;            //输入状态
		memset(line,0,sizeof(line)); //命令缓存
		line_position=0;             //接收位置
		line_curpos=0;               //光标位置
#ifdef MSHELL_USING_ECHO         //回显
	  echo_mode=1;
#else
		echo_mode=0;
#endif
	
#if MSHELL_USING_HISTORY>0
	current_history = history_count =0;
  memset(cmd_history,0,sizeof(cmd_history));
#endif
}

/******************************************************************************
	处理输入 ch
******************************************************************************/
void mshell(char ch)
{
		#if MSHELL_USING_LOGIN > 0
				void login_input_pull(void);
				login_input_pull();//有输入时，登录刷新
	  #endif
	
		if(stat==NOT_INITED)
			mshell_init();
		/*========================================
		 * handle control key
		 * up key  : 0x1b 0x5b 0x41
		 * down key: 0x1b 0x5b 0x42
		 * right key:0x1b 0x5b 0x43
		 * left key: 0x1b 0x5b 0x44
		==========================================*/
		if (ch == 0x1b)
		{
			stat = WAIT_SPEC_KEY;return;
		}
		else if (stat == WAIT_SPEC_KEY)
		{
				if (ch == 0x5b)
				{
						stat = WAIT_FUNC_KEY;return;
				}
				stat = WAIT_NORMAL;
		}
		else if (stat == WAIT_FUNC_KEY)
		{
				stat = WAIT_NORMAL;
				if (ch == 0x41) /* up key */
				{
					#if MSHELL_USING_HISTORY > 0
							mshell_history(-1);
					#endif
					return;
				}
				else if (ch == 0x42) /* down key */
				{
					#if MSHELL_USING_HISTORY > 0
							mshell_history(1);
					#endif
						return;
				}
				else if (ch == 0x44) /* left key */
				{
						if (line_curpos)
						{
								mshell_printf("\b");
								line_curpos --;
						}

						return;
				}
				else if (ch == 0x43) /* right key */
				{
						if (line_curpos < line_position)
						{
								mshell_printf("%c", line[line_curpos]);
								line_curpos ++;
						}

						return;
				}

		}
		/*========================================
		 * handle tab key ：自动补全
		==========================================*/
		else if (ch == '\t')
		{
//            int i;
//            /* move the cursor to the beginning of line */
//            for (i = 0; i < line_curpos; i++)
//                mshell_printf("\b");

//            /* auto complete */
//            shell_auto_complete(&line[0]);
//            /* re-calculate position */
//            line_curpos = line_position = strlen(line);

				return;
		}
		/*========================================
		* handle backspace key :  退格
		==========================================*/
		else if (ch == 0x7f || ch == 0x08)
		{
				/* note that line_curpos >= 0 */
				if (line_curpos == 0)return;
				line_position--;
				line_curpos--;
				if (line_position > line_curpos)
				{
						int i;
						memmove(&line[line_curpos],&line[line_curpos + 1],line_position - line_curpos);
						line[line_position] = 0;
						mshell_printf("\b%s  \b", &line[line_curpos]);
						/* move the cursor to the origin position */
						for (i = line_curpos; i <= line_position; i++)
								mshell_printf("\b");
				}
				else
				{
						mshell_printf("\b \b");
						line[line_position] = 0;
				}
				goto SHELL_INPUT_END;
		}

		/*========================================
		* handle enter key :  一行结尾，处理一条命令
		==========================================*/
		if (ch == '\r' || ch == '\n')
		{
			int result = 0;
			  if( (result = mshell_run_line_first((char*)line)) ==0 )
					mshell_cmd_call((char*)line);
			  if(echo_mode)
					mshell_printf("\n");
				
				#if MSHELL_USING_HISTORY > 0
					#if MSHELL_USING_LOGIN
							if(result!=2)		
								mshell_history(0);//保存历史记录								
					#else
						mshell_history(0);//保存历史记录
					#endif
			  #endif				
					
				mshell_printf(MSHELL_PROMPT);
				memset(line, 0, sizeof(line));
				line_curpos = line_position = 0;
				return;
		}

		
		/*========================================
		* 插入一个字节
		==========================================*/
		if (line_position >= MSHELL_CMD_SIZE)line_position = 0;//太长丢掉
		if (line_curpos < line_position)//在中间插入字符
		{
				int i;
				memmove(&line[line_curpos + 1],&line[line_curpos],line_position - line_curpos);
				line[line_curpos] = ch;
				if (echo_mode)
						rt_kprintf("%s", &line[line_curpos]);

				/* move the cursor to new position */
				for (i = line_curpos; i < line_position; i++)
						rt_kprintf("\b");
		}
		else                           //在末尾插入
		{
				line[line_position] = ch;
				if (echo_mode)
						rt_kprintf("%c", ch);
		}
		line_position ++;
		line_curpos++;
		
		SHELL_INPUT_END:
		/*========================================
		* 如果是login输入的，使用密码输出
		==========================================*/
		#if MSHELL_USING_LOGIN > 0
		{
			char login_input_replace(char*line);
			login_input_replace((char*)line);
		}
		#endif
}

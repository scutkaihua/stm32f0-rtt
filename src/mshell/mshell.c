
#include "mshell_config.h"
/******************************************************************************
// mshell :简单调试终端
//       1.只支持命令调用，不支持变量调用
//       2.支持登录
//       3.支持路径访问
******************************************************************************/

enum input_stat
{
	WAIT_NORMAL,       //正常
	WAIT_SPEC_KEY,     //上下左右键等待
	WAIT_FUNC_KEY,     //上下左右功能键状态
};
static enum input_stat stat;    //输入状态
static U8 echo_mode:1;          //echo 模式
static U8 line[FINSH_CMD_SIZE]; //命令缓存
static U8 line_position;        //接收位置
static U8 line_curpos;          //光标位置

/******************************************************************************
// 解析入参
******************************************************************************/
static void mshell_parameters_parse(void)
{

}
/******************************************************************************
// 历史记录 -1:上一条   0:保存当前命令   1:下一条
// return :1:return  0 continue
******************************************************************************/
#if MSHELL_USING_HISTORY > 0
U16 current_history;
U16 history_count;
char cmd_history[MSHELL_HISTORY_LINES][MSHELL_CMD_SIZE];
static U8 mshell_history(signed char func)
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
            history_count = FINSH_HISTORY_LINES;
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
	}	
	/* 上一条 history */
	else if(func == -1)
	{                
		if (current_history > 0)current_history --;
		else
		{
				current_history = 0;
				return;
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
				else
						return;
		}	
	}else return;
	//显示命令
	memcpy(line, &cmd_history[current_history][0],MSHELL_CMD_SIZE);
	line_curpos = line_position = strlen(line);
	shell_handle_history(shell);
	return 1;
}
#endif
/******************************************************************************
// 调用函数
******************************************************************************/
static void msell_call(void)
{
	/*查找函数*/
	
	/*确定参数个数*/

  /*调用并返回*/
}
/******************************************************************************
// 
******************************************************************************/
void mshell(char data)
{
    /* read one character from device */
    //while (shellGetChar(shandle,&ch) == 1)
    do {
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
							#ifdef FINSH_USING_HISTORY
									mshell_history(1);
							#endif
                return;
            }
            else if (ch == 0x44) /* left key */
            {
                if (shell->line_curpos)
                {
                    rt_kprintf("\b");
                    shell->line_curpos --;
                }

                continue;
            }
            else if (ch == 0x43) /* right key */
            {
                if (shell->line_curpos < shell->line_position)
                {
                    rt_kprintf("%c", shell->line[shell->line_curpos]);
                    shell->line_curpos ++;
                }

                continue;
            }

        }

        /* handle CR key */
        if (ch == '\r')
        {
//            char next = 0;

//            if (shellGetChar(shandle,&ch) == 1)
//                ch = next;
//            else ch = '\r';
        }
        /* handle tab key */
        else if (ch == '\t')
        {
            int i;
            /* move the cursor to the beginning of line */
            for (i = 0; i < shell->line_curpos; i++)
                rt_kprintf("\b");

            /* auto complete */
            shell_auto_complete(&shell->line[0]);
            /* re-calculate position */
            shell->line_curpos = shell->line_position = strlen(shell->line);

            continue;
        }
        /* handle backspace key */
        else if (ch == 0x7f || ch == 0x08)
        {
            /* note that shell->line_curpos >= 0 */
            if (shell->line_curpos == 0)
                continue;

            shell->line_position--;
            shell->line_curpos--;

            if (shell->line_position > shell->line_curpos)
            {
                int i;

                rt_memmove(&shell->line[shell->line_curpos],
                           &shell->line[shell->line_curpos + 1],
                           shell->line_position - shell->line_curpos);
                shell->line[shell->line_position] = 0;

                rt_kprintf("\b%s  \b", &shell->line[shell->line_curpos]);

                /* move the cursor to the origin position */
                for (i = shell->line_curpos; i <= shell->line_position; i++)
                    rt_kprintf("\b");
            }
            else
            {
                rt_kprintf("\b \b");
                shell->line[shell->line_position] = 0;
            }

            continue;
        }

        /* handle end of line, break */
        if (ch == '\r' || ch == '\n')
        {
					  int result = 0;
						#ifdef FINSH_USING_HISTORY
											 mshell_history(0);//保存历史记录
						#endif

#ifdef FINSH_USING_MSH
            if (msh_is_used() == RT_TRUE)
            {
                rt_kprintf("\n");
                msh_exec(shell->line, shell->line_position);
            }
            else
#endif
            {
#ifndef FINSH_USING_MSH_ONLY
                /* add ';' and run the command line */
                shell->line[shell->line_position] = ';';

                if (shell->line_position != 0) {
									/*finsh优先运行，特殊命令*/
									extern int finsh_run_line_first(char*line);
									if((result = finsh_run_line_first(shell->line))==0)
										finsh_run_line(&shell->parser, shell->line);
									if(result>=0){
	#ifdef FINSH_USING_HISTORY
										shell_push_history(shell);
#endif									
									}
								}
                else rt_kprintf("\n");
#endif
            }

            if(result==0)rt_kprintf(FINSH_PROMPT);
            memset(shell->line, 0, sizeof(shell->line));
            line_curpos = line_position = 0;
            break;
        }

        /* it's a large line, discard it */
        if (shell->line_position >= FINSH_CMD_SIZE)
            shell->line_position = 0;

        /* normal character */
        if (shell->line_curpos < shell->line_position)
        {
            int i;

            rt_memmove(&shell->line[shell->line_curpos + 1],
                       &shell->line[shell->line_curpos],
                       shell->line_position - shell->line_curpos);
            shell->line[shell->line_curpos] = ch;
            if (shell->echo_mode)
                rt_kprintf("%s", &shell->line[shell->line_curpos]);

            /* move the cursor to new position */
            for (i = shell->line_curpos; i < shell->line_position; i++)
                rt_kprintf("\b");
        }
        else
        {
					shell->line[shell->line_position] = ch;
					if( memcmp (shell->line,"login ",strlen("login ")) == 0){
						if( ch != ' ')
							rt_kprintf("%c",'*');
						else 
							rt_kprintf("%c",ch);
					}
					else{
            rt_kprintf("%c", ch);
					}
        }

        ch = 0;
        shell->line_position ++;
        shell->line_curpos++;
    } while(0);

}
void mshell_init(void)
{


}
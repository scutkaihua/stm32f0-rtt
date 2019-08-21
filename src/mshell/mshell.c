
#include "mshell_config.h"
#include "mshell.h"
#include "string.h"

extern int  mshell_run_line_first(char*line);
extern void mshell_cmd_call(char*line);
/******************************************************************************
// mshell :�򵥵����ն�
//       1.ֻ֧��������ã���֧�ֱ�������
//       2.֧�ֵ�¼
//       3.֧��·������
******************************************************************************/

enum input_stat
{
	NOT_INITED=-1,     //δ��ʼ��
	WAIT_NORMAL,       //����
	WAIT_SPEC_KEY,     //�������Ҽ��ȴ�
	WAIT_FUNC_KEY,     //�������ҹ��ܼ�״̬
};
static enum input_stat stat=NOT_INITED;//����״̬
static U8 line[MSHELL_CMD_SIZE]; //�����
static U8 line_position;        //����λ��
static U8 line_curpos;          //���λ��
#ifdef MSHELL_USING_ECHO        //����
static U8 echo_mode;            //echo ģʽ
#endif

/******************************************************************************
// ��ʷ��¼ -1:��һ��   0:���浱ǰ����   1:��һ��
// return :1:return  0 continue
******************************************************************************/
#if MSHELL_USING_HISTORY > 0
U16 current_history;
U16 history_count;
char cmd_history[MSHELL_HISTORY_LINES][MSHELL_CMD_SIZE];
static void mshell_history(signed char func)
{
	/* ����һ�� history */
	if(func == 0)
	{
    if (line_position != 0)
    {
        /* push history */
        if (history_count >= MSHELL_HISTORY_LINES)
        {
            /* ���м�¼����*/
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
	/* ��һ�� history */
	else if(func == -1)
	{                
		if (current_history > 0)current_history --;
		else
		{
				current_history = 0;
		}
	}
	/*  ��һ�� history */
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
	//��ʾ����
	memcpy(line, &cmd_history[current_history][0],MSHELL_CMD_SIZE);
	line_curpos = line_position = strlen((const char*)line);
  mshell_printf("\033[2K\r");
  mshell_printf("%s%s", MSHELL_PROMPT,line);
}
#endif

/******************************************************************************
	��ʼ��
******************************************************************************/
static void mshell_init(void)
{
		stat=WAIT_NORMAL;            //����״̬
		memset(line,0,sizeof(line)); //�����
		line_position=0;             //����λ��
		line_curpos=0;               //���λ��
#ifdef MSHELL_USING_ECHO         //����
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
	�������� ch
******************************************************************************/
void mshell(char ch)
{
		#if MSHELL_USING_LOGIN > 0
				void login_input_pull(void);
				login_input_pull();//������ʱ����¼ˢ��
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
		 * handle tab key ���Զ���ȫ
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
		* handle backspace key :  �˸�
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
		* handle enter key :  һ�н�β������һ������
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
								mshell_history(0);//������ʷ��¼								
					#else
						mshell_history(0);//������ʷ��¼
					#endif
			  #endif				
					
				mshell_printf(MSHELL_PROMPT);
				memset(line, 0, sizeof(line));
				line_curpos = line_position = 0;
				return;
		}

		
		/*========================================
		* ����һ���ֽ�
		==========================================*/
		if (line_position >= MSHELL_CMD_SIZE)line_position = 0;//̫������
		if (line_curpos < line_position)//���м�����ַ�
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
		else                           //��ĩβ����
		{
				line[line_position] = ch;
				if (echo_mode)
						rt_kprintf("%c", ch);
		}
		line_position ++;
		line_curpos++;
		
		SHELL_INPUT_END:
		/*========================================
		* �����login����ģ�ʹ���������
		==========================================*/
		#if MSHELL_USING_LOGIN > 0
		{
			char login_input_replace(char*line);
			login_input_replace((char*)line);
		}
		#endif
}

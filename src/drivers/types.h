#ifndef __TYPES_H__
#define __TYPES_H__
#include "stdint.h"

/*������������*/
typedef uint8_t U8;
typedef int8_t S8;
typedef uint16_t U16;
typedef int16_t S16;
typedef uint32_t U32;
typedef int32_t S32;
typedef uint64_t U64;
typedef int64_t S64;
typedef enum{TRUE=1,FALSE=!TRUE}  BOOL;

#ifndef time_t
#define time_t U32
#endif

#ifndef NULL
#define NULL 0
#endif /* NULL */

#ifndef LOW
#define LOW 0
#endif /* LOW */

#ifndef HIGH
#define HIGH 1
#endif /* HIGH */
/*===================================================
                ��ֵ����
====================================================*/
#define KEY_BAI_BI_INDEX    1   //�ڱۿ���
#define KEY_DAO_WEI_INDEX   2   //��λ����
#define KV(type,ch)              (type*0x00200000+ch) //ͨ����ֵ
#define is_key_value(v,type,ch)  if((type*0x00200000+ch)==(v&0x7FFFFFFF))  //�жϼ�ֵ�ǲ���ch��

/*===================================================
                ��������
====================================================*/
//GPIO����������
typedef struct{
	U8 port:4;	                     //0-5 A-F
	U8 pin:4;                        //0-15 pin_0 -- pin_15
	U8 mode:2,otype:2,pp:2,speed:2;	 //ģʽ�������ʽ�������������ٶ�	��stm32������ͬ
}GPIO_Map;


/*===================================================
                ״̬������
====================================================*/
/* ״̬����װ��switch-case(��Ҳ��contiki�ں˷�װ��˼��)ʹ��������:
* 1.���ɶ����Լ���״̬
* 2.ֻ��ע��������ת��������ϸ�����ܵ�ʵ��
* 3.״̬��ʹ��FSM˽�б�����ʹ�������Ա���״̬��������(������������),����״̬�� ������ ��
* 4.�ڿ�����Ļ����ϣ�������״̬����ʱ,��ʹ�� ��ѯϵͳ��ʱ �ķ�ʽ�жϳ�ʱ
* 5.��ʱ�Ķ��壬����ʹ��������״̬������״̬�����ϣ�ʹ�� ��ʱ���� waitus waitms ����
    ʹ���� for() while() �Լ����ǵ� Ƕ�� ѭ��֮��
* note:
*   ״̬��(������)��װ֮�󣬿����� ������̱��(��������) һ��д��
*   �ô���:  ��д��̼򵥣�����ά���������޸������
*/
/*״̬�����ݽṹ*/
#pragma pack(1)
typedef struct{
		U16   			save;	//��״̬����ʹ��for(...){waitms(...)},ʱ���뱣�浱ǰ״̬,����״̬��ɺ�ָ�
		U16   			line;	//״̬������״̬���ڵ��к�
		char*			name;	//״̬�������浱ǰ״̬������
		volatile U32	end;	//��ʱ����ʱ��

		//״̬����ѭ��ʱ������ʹ��˽��static����for(i....)  for(j...)
		S8   i;         //�൱��i
		S8   j;         //�൱��j 
		U16  tmp;       //�൱��tmp
}FSM;
#pragma pack()

//���忪ʼ״̬������һ��switch-case 0
#define Start()             switch(fsm->line){case 0:fsm->name=NULL;
	
//����һ��״̬: ��ʹ��case __LINE__ ����λ
#define State(sname)   \
	/*��ʱǰ״̬�ָ�*/             if(fsm->save)fsm->line=fsm->save;fsm->save=0; goto Exp; \
	/*��ǩ,������goto����ת������*/sname: fsm->end=0; fsm->name=#sname;fsm->line=__LINE__;fsm->save=0;goto Exp; \
	/*switch-case,����ʱ��λ���� */case __LINE__:
	
//����һ��Default״̬
#define Default() default:Exp:{}}
	
//��λ״̬��
#define reset_fsm()    fsm->save=fsm->line=fsm->end=0;goto Exp
	
//״̬������ʱ(������): ��ʹ������ case ��ʵ��(Ҳ����ʹ������������״̬��ʵ��delay����)
//now new �������� __LINE__��ͻ
#define Waitx(us,now,new)		 \
	/*������ʱ*/                   fsm->end=fsm_time+us; \
  /*���浱ǰ״̬*/               if(fsm->save==0)fsm->save=fsm->line; \
  /*������ʱ״̬*/               fsm->line=__LINE__+now; goto Exp;case __LINE__+now: \
  /*�ж��Ƿ�ʱ*/               if((fsm_time-fsm->end)>0x80000000)goto Exp; \
  /*��ʱ��*/                     fsm->line=__LINE__+new;fsm->end=0;case __LINE__+new:

/*��ʱms*/
#define waitms(ms)               Waitx((ms*1000),10000,20000)
#define waitmsx(ms)              Waitx(ms,10000,20000)

/*��ʱus>100us*/
#define waitus(us)               Waitx(us,10000,20000)
/*����״̬����ǰʱ��*/
#define fsm_time_set(t)          static U32 fsm_time=0; fsm_time=t
#define fsm_time_add(t)          static U32 fsm_time=0; fsm_time+=t
/*===================================================
               os ��ʱ
====================================================*/
#define os_delay(name,time) 		 etimer_set(&(et_##name),((U32)time)/(1000/CLOCK_SECOND));PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_##name))

/*��auto start �߳�д��*/
#define AUTOSTART_THREAD_WITH_TIMEOUT(name) \
																						static struct etimer et_##name; \
																						PROCESS(thread_##name, #name); \
																						AUTOSTART_PROCESSES(thread_##name); \
																						PROCESS_THREAD(thread_##name, ev, data)  
#define AUTOSTART_THREAD_WITHOUT_TIMEOUT(name) \
																						PROCESS(thread_##name, #name); \
																						AUTOSTART_PROCESSES(thread_##name); \
																						PROCESS_THREAD(thread_##name, ev, data) 

#endif

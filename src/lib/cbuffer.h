#ifndef ___H__
#define ___H__
/**************************************************************************************
	��������-ͨѶ��
***************************************************************************************/
#include "types.h"

typedef struct
{
    char *pbuf;
    unsigned int maxlen;
    unsigned int rptr;
    unsigned int wptr;
} CBuffer;

#include <stdlib.h>

/*�� cbuf ���� maxlen �ֽڴ�С*/
BOOL cbuffer_init(CBuffer* cbuf,unsigned int maxlen);

/*�ͷ�cbuf*/
BOOL cbuffer_free(CBuffer* cbuf);

/*��ȡcbuf һ���ֽڷ��� readdata*/
BOOL cbuffer_read(CBuffer* cbuf,char* readdata);

/*дһ���ֽ�writedata ��cbuf*/
BOOL cbuffer_write(CBuffer* cbuf,char writedata);

/*���cbuf�Ƿ���*/
BOOL cbuffer_isfull(CBuffer* cbuf);

// /*���ݳ��ȴ�С*/
// int cbuffer_size(CBuffer* cbuf);

/*���cbuf����*/
BOOL cbuffer_clear(CBuffer* cbuf);

/*��block
* size:Ҫ��ȡ�Ĵ�С
* return :��ȡ���Ĵ�С
*/
int cbuffer_readbk(CBuffer* cbuf,char* dst,int size);
//����д����ֽ���
int cbuffer_writebk(CBuffer* cbuf,const char* src,int size);

#define cbuffer_writeblock(x,y,z) 	{while((z)>0&&cbuffer_write(&(x),*((y)+(z)))!=FALSE)(z)--;}

#endif

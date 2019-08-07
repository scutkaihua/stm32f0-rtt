#ifndef ___H__
#define ___H__
/**************************************************************************************
	环形数组-通讯用
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

/*给 cbuf 分配 maxlen 字节大小*/
BOOL cbuffer_init(CBuffer* cbuf,unsigned int maxlen);

/*释放cbuf*/
BOOL cbuffer_free(CBuffer* cbuf);

/*读取cbuf 一个字节放在 readdata*/
BOOL cbuffer_read(CBuffer* cbuf,char* readdata);

/*写一个字节writedata 到cbuf*/
BOOL cbuffer_write(CBuffer* cbuf,char writedata);

/*检查cbuf是否满*/
BOOL cbuffer_isfull(CBuffer* cbuf);

// /*数据长度大小*/
// int cbuffer_size(CBuffer* cbuf);

/*清除cbuf数据*/
BOOL cbuffer_clear(CBuffer* cbuf);

/*读block
* size:要读取的大小
* return :读取到的大小
*/
int cbuffer_readbk(CBuffer* cbuf,char* dst,int size);
//返回写入的字节数
int cbuffer_writebk(CBuffer* cbuf,const char* src,int size);

#define cbuffer_writeblock(x,y,z) 	{while((z)>0&&cbuffer_write(&(x),*((y)+(z)))!=FALSE)(z)--;}

#endif

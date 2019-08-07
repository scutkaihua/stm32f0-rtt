#ifndef __LIB_H__
#define __LIB_H__
#include "types.h"


/**
	* 比较两个数组是否相等
	* return :TRUE ：相等  FALSE:不等
  */
BOOL buffer_cmp(U8* pbuf,volatile U8* pbuf1, U8 len);

/*累加和*/
U8 cs8(volatile U8*data,U32 len);


//决断time out
#define istimeout(now32,end32)  ((now32-end32)<0x80000000)
#endif


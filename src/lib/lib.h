#ifndef __LIB_H__
#define __LIB_H__
#include "types.h"


/**
	* �Ƚ����������Ƿ����
	* return :TRUE �����  FALSE:����
  */
BOOL buffer_cmp(U8* pbuf,volatile U8* pbuf1, U8 len);

/*�ۼӺ�*/
U8 cs8(volatile U8*data,U32 len);


//����time out
#define istimeout(now32,end32)  ((now32-end32)<0x80000000)
#endif


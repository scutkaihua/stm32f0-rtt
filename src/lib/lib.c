#include "lib.h"



/**
	* 比较两个数组是否相等
	* return :TRUE ：相等  FALSE:不等
  */
BOOL buffer_cmp(U8* pbuf,volatile U8* pbuf1, U8 len)
{
  while(len--)
  {
    if(*pbuf != *pbuf1)
    {
      return FALSE;
    }
    pbuf++;
    pbuf1++;
  }
  return TRUE;
}

/*累加和*/
U8 cs8(volatile U8*data,U32 len)
{
	U8 cs = 0;
  U32 i=0;
	for(;i<len;i++)cs+=data[i];
	return cs;
}

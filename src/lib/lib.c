#include "lib.h"



/**
	* �Ƚ����������Ƿ����
	* return :TRUE �����  FALSE:����
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

/*�ۼӺ�*/
U8 cs8(volatile U8*data,U32 len)
{
	U8 cs = 0;
  U32 i=0;
	for(;i<len;i++)cs+=data[i];
	return cs;
}

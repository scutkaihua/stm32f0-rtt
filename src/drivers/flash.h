#ifndef  __FLASH_H
#define  __FLASH_H

#include "stm32f0xx.h" 

typedef __IO uint32_t  vu32;

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;



#define RS485_ADDR	        0x8000000+(63-1)*1024		  //倒数第二页  保存485地址
#define FIRMWARE_updata	    0x8000000+(59-1)*1024		  //保存固件更新标志位



void WriteFlsh(uint32_t WriteAddress, uint8_t *WriteBuf, int32_t WriteNum, uint32_t WriteAddr_page);
void ReadFlsh(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum,uint32_t WriteAddr_page);


TestStatus Buffercmp(uint8_t* pBuffer,volatile uint8_t* pBuffer1, uint8_t BufferLength);




#endif

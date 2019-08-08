#include "flash.h"


void ld_flash_write(uint32_t WriteAddress, uint8_t *WriteBuf, int32_t WriteNum, uint32_t WriteAddr_page) 
{
  int DataNum = 0;
	WriteAddress=(vu32)WriteAddr_page + WriteAddress; 
	FLASH_Unlock();
	  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
	FLASH_ErasePage(WriteAddr_page);
	while(DataNum<WriteNum)
	{
   FLASH_ProgramHalfWord(WriteAddress,*(WriteBuf+DataNum));
   DataNum++;
	 WriteAddress+=4;
	}
	FLASH_Lock();	
}
void ld_flash_read(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum,uint32_t WriteAddr_page) 
{
	int DataNum = 0; 
 
	ReadAddress = (vu32)WriteAddr_page + ReadAddress; 
	while(DataNum<ReadNum)
	{
		*(ReadBuf+DataNum)=*(vu32*)(ReadAddress);
		DataNum++;	
		ReadAddress+=4;
	}
}
/////////////////////////////////////////////////////////////////////////////////
//void WriteFlsh(uint32_t WriteAddress, volatile uint8_t *WriteBuf, int32_t WriteNum) 
//{
//  int DataNum = 0;
//	WriteAddress=(vu32)EEP_ADDR + WriteAddress; 
//	FLASH_Unlock();
//	  /* Clear pending flags (if any) */  
//  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
//	FLASH_ErasePage(EEP_ADDR);
//	while(DataNum<WriteNum)
//	{
//   FLASH_ProgramHalfWord(WriteAddress,*(WriteBuf+DataNum));
//   DataNum++;
//	 WriteAddress+=4;
//	}
//	FLASH_Lock();	
//}

//void ReadFlsh(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum) 
//{
//	int DataNum = 0; 
// 
//	ReadAddress = (vu32)EEP_ADDR + ReadAddress; 
//	while(DataNum<ReadNum)
//	{
//		*(ReadBuf+DataNum)=*(vu32*)(ReadAddress);
//		DataNum++;	
//		ReadAddress+=4;
//	}

//}
/////////////////////////////////////////////////////////////////////////////





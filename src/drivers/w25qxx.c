/*========================================================

* file name 		:	flash.c
* Author 			:	kaihua.yang
* Version			��	v1.00
* Date				��	5/31/2017
* Description		��	flash��

*********************************************************
*��Ӳ���йز����� Configure or interface����
*********************************************************
========================================================*/
#include "driver_config_types.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "cpu.h"

/*************************************
	����
**************************************/
#ifdef __flash_DEBUG
#define flash_printf gt_printf
#else
#define flash_printf(...)
#endif

/**************************************

	Configure  or  interface
***************************************/
extern t_spi_flash_map spi_flash_map[];
extern const unsigned char spi_flash_number; //flash оƬ����
extern const unsigned char current_flash_port;
extern void ld_iwdg_reload(void);
/*gedev spi�翴��*/
extern int ld_spi_init(U8 index);
extern int ld_spi_transfer(U8 index ,U8 sendbyte,U8 *readbyte);
extern int ld_spi_nss(U8 index,U8 level);

/**************************************
	spi �����ӿ�
***************************************/

#define SPI_FLASH_CS_LOW(index)		ld_spi_nss(index,0)
#define SPI_FLASH_CS_HIGH(index)  ld_spi_nss(index,1)

/*************************************
	spi flash function
**************************************/
/*w25q16������*/
#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg		0x05
#define W25X_WriteStatusReg		0x01
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_ChipErase			0xC7
#define W25X_PowerDown			0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F

#define WIP_Flag                0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte              0xA5


/**
  * @function  static U8 SPIx_ReadWriteByte(U8 TxData)
  * @brief     SPIx ��дһ���ֽ�
	* @param     spi   :Ƭ��spi�˿�
  * @param     TxData:Ҫд����ֽ�
  * @retval    ����ֵ:��ȡ�����ֽ�
  */
static U8 SPIx_ReadWriteByte(U8 spi,U8 TxData)
{		
	U8 read=0;			
	ld_spi_transfer(spi,TxData,&read);
	return read;
}

/**
  * @function  static U8 Spi_Flash_ReadSR(void)
  * @brief     ��ȡSPI_FLASH��״̬�Ĵ���
  *            BIT7  6   5   4   3   2   1   0
  *            SPR   RV  TB BP2 BP1 BP0 WEL BUSY
  *            SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
  *            TB,BP2,BP1,BP0:FLASH����д��������
  *            WEL:дʹ������
  *            BUSY:æ���λ(1,æ;0,����)
  *            Ĭ��:0x00
  * @param     None
  * @retval    None
  */
static U8 Spi_Flash_ReadSR(U8 spi)
{  
	U8 byte=0;   
	SPI_FLASH_CS_LOW(spi);                        //ʹ������   
	SPIx_ReadWriteByte(spi,W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPIx_ReadWriteByte(spi,0Xff);             //��ȡһ���ֽ�  
	SPI_FLASH_CS_HIGH(spi);                       //ȡ��Ƭѡ     
	return byte;   
}

/**
  * @function  static void Spi_Flash_Write_Enable(void)
  * @brief     SPI_FLASHдʹ��	, ��WEL��λ
  * @param     None
  * @retval    None
  */
static void Spi_Flash_Write_Enable(U8 spi)   
{
	SPI_FLASH_CS_LOW(spi);                            //ʹ������   
  SPIx_ReadWriteByte(spi,W25X_WriteEnable);      //����дʹ��  
	SPI_FLASH_CS_HIGH(spi);                            //ȡ��Ƭѡ     	      
}

/**
  * @function  static void SPI_Flash_Wait_Busy(void)
  * @brief     �ȴ�SPI FLASH ����
  * @param     None
  * @retval    None
  */
static void SPI_Flash_Wait_Busy(U8 spi)   
{   
	while ((Spi_Flash_ReadSR(spi)&0x01)==0x01);   // �ȴ�BUSYλ���
}

/**
  * @function  U16 Spi_Flash_ReadID(void)
  * @brief     ��ȡоƬID W25X16��ID:0XEF14
  * @param     None
  * @retval    Temp
  */
static U16 Spi_Flash_ReadID(U8 spi)
{
	U16 Temp = 0;	  
	SPI_FLASH_CS_LOW(spi);				    
	SPIx_ReadWriteByte(spi,0x90);//���Ͷ�ȡID����	    
	SPIx_ReadWriteByte(spi,0x00); 	    
	SPIx_ReadWriteByte(spi,0x00); 	    
	SPIx_ReadWriteByte(spi,0x00); 	 			   
	Temp|=SPIx_ReadWriteByte(spi,0xFF)<<8;  
	Temp|=SPIx_ReadWriteByte(spi,0xFF);	 
	SPI_FLASH_CS_HIGH(spi);				    
	return Temp;
}

/**
  * @function  void Spi_Flash_Erase_Chip(void)
  * @brief     ��������оƬ  W25X16:25s  W25X32:40s  W25X64:40s
  * @param     None
  * @retval    None
  */
static void Spi_Flash_Erase_Chip(U8 spi)
{                                             
    Spi_Flash_Write_Enable(spi);                  //SET WEL 
    SPI_Flash_Wait_Busy(spi);
  	SPI_FLASH_CS_LOW(spi);                        //ʹ������   
    SPIx_ReadWriteByte(spi,W25X_ChipErase);        //����Ƭ��������  
	  SPI_FLASH_CS_HIGH(spi);                       //ȡ��Ƭѡ     	      
	  SPI_Flash_Wait_Busy(spi);   				           //�ȴ�оƬ��������
}

/**
  * @function  s32_t Spi_Flash_Read(U32 ReadAddr,U32 NumByteToRead,U8 * pBuffer)
  * @brief     ��ָ������ʼ��ַ����ȡָ�����ȵ����� (�ļ�ϵͳ�ӿں���)
  * @param     pBuffer:���ݴ洢��   ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
  *            NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
  * @retval    0
  */
static int Spi_Flash_Read(U8 spi,unsigned int ReadAddr,unsigned int NumByteToRead,unsigned char * pBuffer)
{ 
 	U16 i;
	SPI_FLASH_CS_LOW(spi);                          //ʹ������   
	SPIx_ReadWriteByte(spi,W25X_ReadData);           //���Ͷ�ȡ����   
	SPIx_ReadWriteByte(spi,(U8)((ReadAddr)>>16));  //����24bit��ַ    
	SPIx_ReadWriteByte(spi,(U8)((ReadAddr)>>8));   
	SPIx_ReadWriteByte(spi,(U8)ReadAddr);   
	for(i=0;i<NumByteToRead;i++){ 
		 pBuffer[i]=SPIx_ReadWriteByte(spi,0XFF);      //ѭ������
	}
	SPI_FLASH_CS_HIGH(spi);                         //ȡ��Ƭѡ
  return 0;		
}

/**
  * @function  s32_t Spi_Flash_Write(U32 WriteAddr,U32 NumByteToWrite,U8 * pBuffer)
  * @brief     ��ָ������ʼ��ַ����ʼд�����256�ֽڵ����� (�ļ�ϵͳ�ӿں���)
  * @param     pBuffer: ���ݴ洢��  WriteAddr:��ʼд��ĵ�ַ(24bit)
  *            NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���
  * @retval    0
  */
static int Spi_Flash_Page_Write(U8 spi,unsigned int WriteAddr,unsigned int NumByteToWrite,unsigned char * pBuffer)
{
 	U16 i;  
  Spi_Flash_Write_Enable(spi);                  //SET WEL 
	SPI_FLASH_CS_LOW(spi);                        //ʹ������   
  SPIx_ReadWriteByte(spi,W25X_PageProgram);      //����дҳ����   
  SPIx_ReadWriteByte(spi,(U8)((WriteAddr)>>16)); //����24bit��ַ    
  SPIx_ReadWriteByte(spi,(U8)((WriteAddr)>>8));   
  SPIx_ReadWriteByte(spi,(U8)WriteAddr);   
  for(i=0;i<NumByteToWrite;i++) SPIx_ReadWriteByte(spi,pBuffer[i]); //ѭ��д��  
	SPI_FLASH_CS_HIGH(spi);                                          //ȡ��Ƭѡ 
	SPI_Flash_Wait_Busy(spi);					                              //�ȴ�д�����
	return 0;
}
//�Զ���ҳ
#define PageSize 256
static int Spi_Flash_Write(U8 spi,unsigned int WriteAddr,unsigned int NumByteToWrite,unsigned char * pBuffer)
{
	int o = WriteAddr%PageSize;
	int w = 0;
	int ww = NumByteToWrite;
	while(ww>0)
	{
		w = (PageSize-o)>ww?ww:(PageSize-o);
	  Spi_Flash_Page_Write(spi,WriteAddr,w,pBuffer);
		o = 0;
		WriteAddr+=w;
		ww-=w;
		pBuffer+=w;
	}
	return NumByteToWrite;
}



/**
  * @function  s32_t Spi_Flash_Erase(U32 Addr,U32 Num)
  * @brief     ����������� (�ļ�ϵͳ�ӿں���)
  * @param     Addr ���������ʼ��ַ  Size: ������С[bytes]
  * @retval    None
  */
static int Spi_Flash_Erase(U8 spi,unsigned int Addr,unsigned int Size)
{   
	U32 secpos;
	U32 Address;
	U32 i;
	Size /=4096;
	secpos = Addr/4096;               //������ַ (��������ĵ�ַ���ڵڼ�������)
	for(i=0 ; i < Size ; i++) {
		ld_iwdg_reload();
		Address = secpos * 4096;        //��������������������ʼ��ַ
		Spi_Flash_Write_Enable(spi);                   //SET WEL 	 
		SPI_Flash_Wait_Busy(spi);   
		SPI_FLASH_CS_LOW(spi);                         //ʹ������   
		SPIx_ReadWriteByte(spi,W25X_SectorErase);       //������������ָ�� 
		SPIx_ReadWriteByte(spi,(U8)((Address)>>16));  //����24bit��ַ    
		SPIx_ReadWriteByte(spi,(U8)((Address)>>8));   
		SPIx_ReadWriteByte(spi,(U8)Address);  
		SPI_FLASH_CS_HIGH(spi);                        //ȡ��Ƭѡ     	      
		SPI_Flash_Wait_Busy(spi);   				            //�ȴ��������
		secpos++;	
		
	}
	return 0;
}

/*************************************
	private functions
**************************************/
// static int f_mutex[4]={0};
// #define f_lock(spi) {   \
//                         if(f_mutex[spi]==0)f_mutex[spi] = gt_mutex_init(); \
//                         gt_mutex_lock(f_mutex[spi]); \
//                     }
// #define f_unlock(spi)   gt_mutex_unlock(f_mutex[spi]);

#define f_lock(spi)
#define f_unlock(spi)
/**************************************
	public
**************************************/

/*��ʼ��flash,spi:[Ƭ��spi����]  return :[true or false]*/
int ld_spi_flash_init(U8 spi)
{
	return ld_spi_init(spi);
}
int ld_spi_flash_readID(U8 spi)
{
	return (int) Spi_Flash_ReadID(spi);
}
/*��ȡ������:0 �ɹ�   <0 ʧ��*/
int ld_spi_flash_read(U8 spi, U32 address, U8*data,U32 len)
{
	f_lock(spi)
	Spi_Flash_Read(spi, address, len ,data);
	f_unlock(spi)
	return 0;
}

/*д��, ����:0 �ɹ� <0 ʧ��*/
int ld_spi_flash_write(U8 spi, U32 address, U8*data,U32 len)
{
	f_lock(spi)
	Spi_Flash_Write(spi, address,  len, data);
	f_unlock(spi)
	return 0;
}

/*����,����:0 �ɹ� <0 ʧ��,size:������С[bytes]*/
int ld_spi_flash_erase(U8 spi,U32 address,U32 size)
{   
	f_lock(spi)
	Spi_Flash_Erase(spi,address,size);
	f_unlock(spi)
	return 0;
}

int ld_spi_flash_chip_erase(U8 spi)
{
	 Spi_Flash_Erase_Chip(spi);
	 return 1;
}

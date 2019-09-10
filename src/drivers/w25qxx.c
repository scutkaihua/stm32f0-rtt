/*========================================================

* file name 		:	flash.c
* Author 			:	kaihua.yang
* Version			：	v1.00
* Date				：	5/31/2017
* Description		：	flash动

*********************************************************
*与硬件有关部分在 Configure or interface给出
*********************************************************
========================================================*/
#include "driver_config_types.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "cpu.h"

/*************************************
	调试
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
extern const unsigned char spi_flash_number; //flash 芯片个数
extern const unsigned char current_flash_port;
extern void ld_iwdg_reload(void);
/*gedev spi如看看*/
extern int ld_spi_init(U8 index);
extern int ld_spi_transfer(U8 index ,U8 sendbyte,U8 *readbyte);
extern int ld_spi_nss(U8 index,U8 level);

/**************************************
	spi 操作接口
***************************************/

#define SPI_FLASH_CS_LOW(index)		ld_spi_nss(index,0)
#define SPI_FLASH_CS_HIGH(index)  ld_spi_nss(index,1)

/*************************************
	spi flash function
**************************************/
/*w25q16命令码*/
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
  * @brief     SPIx 读写一个字节
	* @param     spi   :片上spi端口
  * @param     TxData:要写入的字节
  * @retval    返回值:读取到的字节
  */
static U8 SPIx_ReadWriteByte(U8 spi,U8 TxData)
{		
	U8 read=0;			
	ld_spi_transfer(spi,TxData,&read);
	return read;
}

/**
  * @function  static U8 Spi_Flash_ReadSR(void)
  * @brief     读取SPI_FLASH的状态寄存器
  *            BIT7  6   5   4   3   2   1   0
  *            SPR   RV  TB BP2 BP1 BP0 WEL BUSY
  *            SPR:默认0,状态寄存器保护位,配合WP使用
  *            TB,BP2,BP1,BP0:FLASH区域写保护设置
  *            WEL:写使能锁定
  *            BUSY:忙标记位(1,忙;0,空闲)
  *            默认:0x00
  * @param     None
  * @retval    None
  */
static U8 Spi_Flash_ReadSR(U8 spi)
{  
	U8 byte=0;   
	SPI_FLASH_CS_LOW(spi);                        //使能器件   
	SPIx_ReadWriteByte(spi,W25X_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=SPIx_ReadWriteByte(spi,0Xff);             //读取一个字节  
	SPI_FLASH_CS_HIGH(spi);                       //取消片选     
	return byte;   
}

/**
  * @function  static void Spi_Flash_Write_Enable(void)
  * @brief     SPI_FLASH写使能	, 将WEL置位
  * @param     None
  * @retval    None
  */
static void Spi_Flash_Write_Enable(U8 spi)   
{
	SPI_FLASH_CS_LOW(spi);                            //使能器件   
  SPIx_ReadWriteByte(spi,W25X_WriteEnable);      //发送写使能  
	SPI_FLASH_CS_HIGH(spi);                            //取消片选     	      
}

/**
  * @function  static void SPI_Flash_Wait_Busy(void)
  * @brief     等待SPI FLASH 空闲
  * @param     None
  * @retval    None
  */
static void SPI_Flash_Wait_Busy(U8 spi)   
{   
	while ((Spi_Flash_ReadSR(spi)&0x01)==0x01);   // 等待BUSY位清空
}

/**
  * @function  U16 Spi_Flash_ReadID(void)
  * @brief     读取芯片ID W25X16的ID:0XEF14
  * @param     None
  * @retval    Temp
  */
static U16 Spi_Flash_ReadID(U8 spi)
{
	U16 Temp = 0;	  
	SPI_FLASH_CS_LOW(spi);				    
	SPIx_ReadWriteByte(spi,0x90);//发送读取ID命令	    
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
  * @brief     擦除整个芯片  W25X16:25s  W25X32:40s  W25X64:40s
  * @param     None
  * @retval    None
  */
static void Spi_Flash_Erase_Chip(U8 spi)
{                                             
    Spi_Flash_Write_Enable(spi);                  //SET WEL 
    SPI_Flash_Wait_Busy(spi);
  	SPI_FLASH_CS_LOW(spi);                        //使能器件   
    SPIx_ReadWriteByte(spi,W25X_ChipErase);        //发送片擦除命令  
	  SPI_FLASH_CS_HIGH(spi);                       //取消片选     	      
	  SPI_Flash_Wait_Busy(spi);   				           //等待芯片擦除结束
}

/**
  * @function  s32_t Spi_Flash_Read(U32 ReadAddr,U32 NumByteToRead,U8 * pBuffer)
  * @brief     在指定的起始地址处读取指定长度的数据 (文件系统接口函数)
  * @param     pBuffer:数据存储区   ReadAddr:开始读取的地址(24bit)
  *            NumByteToRead:要读取的字节数(最大65535)
  * @retval    0
  */
static int Spi_Flash_Read(U8 spi,unsigned int ReadAddr,unsigned int NumByteToRead,unsigned char * pBuffer)
{ 
 	U16 i;
	SPI_FLASH_CS_LOW(spi);                          //使能器件   
	SPIx_ReadWriteByte(spi,W25X_ReadData);           //发送读取命令   
	SPIx_ReadWriteByte(spi,(U8)((ReadAddr)>>16));  //发送24bit地址    
	SPIx_ReadWriteByte(spi,(U8)((ReadAddr)>>8));   
	SPIx_ReadWriteByte(spi,(U8)ReadAddr);   
	for(i=0;i<NumByteToRead;i++){ 
		 pBuffer[i]=SPIx_ReadWriteByte(spi,0XFF);      //循环读数
	}
	SPI_FLASH_CS_HIGH(spi);                         //取消片选
  return 0;		
}

/**
  * @function  s32_t Spi_Flash_Write(U32 WriteAddr,U32 NumByteToWrite,U8 * pBuffer)
  * @brief     在指定的起始地址处开始写入最大256字节的数据 (文件系统接口函数)
  * @param     pBuffer: 数据存储区  WriteAddr:开始写入的地址(24bit)
  *            NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数
  * @retval    0
  */
static int Spi_Flash_Page_Write(U8 spi,unsigned int WriteAddr,unsigned int NumByteToWrite,unsigned char * pBuffer)
{
 	U16 i;  
  Spi_Flash_Write_Enable(spi);                  //SET WEL 
	SPI_FLASH_CS_LOW(spi);                        //使能器件   
  SPIx_ReadWriteByte(spi,W25X_PageProgram);      //发送写页命令   
  SPIx_ReadWriteByte(spi,(U8)((WriteAddr)>>16)); //发送24bit地址    
  SPIx_ReadWriteByte(spi,(U8)((WriteAddr)>>8));   
  SPIx_ReadWriteByte(spi,(U8)WriteAddr);   
  for(i=0;i<NumByteToWrite;i++) SPIx_ReadWriteByte(spi,pBuffer[i]); //循环写数  
	SPI_FLASH_CS_HIGH(spi);                                          //取消片选 
	SPI_Flash_Wait_Busy(spi);					                              //等待写入结束
	return 0;
}
//自动换页
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
  * @brief     擦除多个扇区 (文件系统接口函数)
  * @param     Addr ：块擦除开始地址  Size: 擦除大小[bytes]
  * @retval    None
  */
static int Spi_Flash_Erase(U8 spi,unsigned int Addr,unsigned int Size)
{   
	U32 secpos;
	U32 Address;
	U32 i;
	Size /=4096;
	secpos = Addr/4096;               //扇区地址 (计算给定的地址处于第几个扇区)
	for(i=0 ; i < Size ; i++) {
		ld_iwdg_reload();
		Address = secpos * 4096;        //根据扇区计算扇区的起始地址
		Spi_Flash_Write_Enable(spi);                   //SET WEL 	 
		SPI_Flash_Wait_Busy(spi);   
		SPI_FLASH_CS_LOW(spi);                         //使能器件   
		SPIx_ReadWriteByte(spi,W25X_SectorErase);       //发送扇区擦除指令 
		SPIx_ReadWriteByte(spi,(U8)((Address)>>16));  //发送24bit地址    
		SPIx_ReadWriteByte(spi,(U8)((Address)>>8));   
		SPIx_ReadWriteByte(spi,(U8)Address);  
		SPI_FLASH_CS_HIGH(spi);                        //取消片选     	      
		SPI_Flash_Wait_Busy(spi);   				            //等待擦除完成
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

/*初始化flash,spi:[片上spi索引]  return :[true or false]*/
int ld_spi_flash_init(U8 spi)
{
	return ld_spi_init(spi);
}
int ld_spi_flash_readID(U8 spi)
{
	return (int) Spi_Flash_ReadID(spi);
}
/*读取，返回:0 成功   <0 失败*/
int ld_spi_flash_read(U8 spi, U32 address, U8*data,U32 len)
{
	f_lock(spi)
	Spi_Flash_Read(spi, address, len ,data);
	f_unlock(spi)
	return 0;
}

/*写入, 返回:0 成功 <0 失败*/
int ld_spi_flash_write(U8 spi, U32 address, U8*data,U32 len)
{
	f_lock(spi)
	Spi_Flash_Write(spi, address,  len, data);
	f_unlock(spi)
	return 0;
}

/*擦除,返回:0 成功 <0 失败,size:擦除大小[bytes]*/
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

#ifndef __LFS_CONFIG_H__
#define __LFS_CONFIG_H__

//https://blog.csdn.net/diaoxuesong/article/details/87726863



//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//<h>�豸
//<o>����С(byte)
//<i> ÿ�ζ�ȡ���ֽ��������Ա��������Ԫ���Ը������ܣ������ֵ�����˶�����Ĵ�С����ֵ̫������������ڴ����ġ�
#define SPI_FLASH_READ_SIZE  32

//<o>��̴�С(byte)
//<i> ÿ��д����ֽ��������Ա�����д��Ԫ���Ը������ܣ������ֵ������д����Ĵ�С��������read_size������������ֵ̫������������ڴ����ġ�
#define SPI_FLASH_PROG_SIZE  32

//<o>���С(K) 
#define BLOCK_SIZE 4

//<o>�ܴ�С(K) 
#define SPI_FLASH_FULL_SIZE  512

//<o>�����С(byte)
#define SPI_FLASH_CACHE_SIZE 32

//<o>Ԥ�����(block)
//<i>�����ʱ��Ԥ����ȣ������ʱÿ�β������ٸ��飩�������ֵ����Ϊ32������������1024��ʾÿ��Ԥ��1024��block�����ֵ�����ڴ�����Ӱ�첻����Ϊ����Ӧ��lookahead_buffer ��ʹ��1bit����һ��block��
#define SPI_FLASH_LOOKAHEAD_SIZE 16

//<o>spi�豸��  <1=>spi1 <2=>spi2
#define SPI_FLASH_PORT  1
//</h>



//<h> ��ӡ
//<e>ʹ��mshell��ӡ
#define USING_MSHELL_PRINTF 0
//</e>

#if USING_MSHELL_PRINTF > 0
#define printf mshell_printf
#else 
#define printf(...)
#endif

//<c>ʹ��trace
//#define LFS_YES_TRACE
//</c>

//<c>����debug
#define LFS_NO_DEBUG
//</c>

//<c>����warn
#define LFS_NO_WARN
//</c>

//<c>����error
#define LFS_NO_ERROR
//</c>
//</h>

//<c>����assert
#define LFS_NO_ASSERT
//</c>

//<o>�ļ�������(byte)
#define LFS_NAME_MAX 20

//<o>����ļ�����(Kbyte)
//<i> С��2GByte
#define LFS_FILE_MAX    128 *1024

#endif


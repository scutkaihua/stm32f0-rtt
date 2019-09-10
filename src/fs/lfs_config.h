#ifndef __LFS_CONFIG_H__
#define __LFS_CONFIG_H__

//https://blog.csdn.net/diaoxuesong/article/details/87726863



//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//<h>设备
//<o>读大小(byte)
//<i> 每次读取的字节数，可以比物理读单元大以改善性能，这个数值决定了读缓存的大小，但值太大会带来更多的内存消耗。
#define SPI_FLASH_READ_SIZE  32

//<o>编程大小(byte)
//<i> 每次写入的字节数，可以比物理写单元大以改善性能，这个数值决定了写缓存的大小，必须是read_size的整数倍，但值太大会带来更多的内存消耗。
#define SPI_FLASH_PROG_SIZE  32

//<o>块大小(K) 
#define BLOCK_SIZE 4

//<o>总大小(K) 
#define SPI_FLASH_FULL_SIZE  512

//<o>缓存大小(byte)
#define SPI_FLASH_CACHE_SIZE 32

//<o>预测深度(block)
//<i>块分配时的预测深度（分配块时每次步进多少个块），这个数值必须为32的整数倍，如1024表示每次预测1024个block。这个值对于内存消耗影响不大，因为它对应的lookahead_buffer 中使用1bit代表一个block。
#define SPI_FLASH_LOOKAHEAD_SIZE 16

//<o>spi设备号  <1=>spi1 <2=>spi2
#define SPI_FLASH_PORT  1
//</h>



//<h> 打印
//<e>使用mshell打印
#define USING_MSHELL_PRINTF 0
//</e>

#if USING_MSHELL_PRINTF > 0
#define printf mshell_printf
#else 
#define printf(...)
#endif

//<c>使能trace
//#define LFS_YES_TRACE
//</c>

//<c>禁用debug
#define LFS_NO_DEBUG
//</c>

//<c>禁用warn
#define LFS_NO_WARN
//</c>

//<c>禁用error
#define LFS_NO_ERROR
//</c>
//</h>

//<c>禁用assert
#define LFS_NO_ASSERT
//</c>

//<o>文件名长度(byte)
#define LFS_NAME_MAX 20

//<o>最大文件长度(Kbyte)
//<i> 小于2GByte
#define LFS_FILE_MAX    128 *1024

#endif


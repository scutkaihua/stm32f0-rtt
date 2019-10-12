#include "lfs.h"
#include "dev.h"
#include "mshell.h"
// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;

int counter[SPI_FLASH_FULL_SIZE/BLOCK_SIZE]={0};

int user_provided_block_device_read(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size) {

    //ASSERT(block < c->block_count);
		ld_spi_flash_read((U8)((int)c->context),(U32)(block * c->block_size + off),(U8 *)buffer,  size);
    return 0;
}

int user_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {
			counter[block]++;
    //ASSERT(block < c->block_count);
    ld_spi_flash_write((U8)((int)c->context),(U32)(block * c->block_size + off),(U8 *)buffer,  size);
    return 0;
}

int user_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block) {
	  counter[block]++;
    //ASSERT(block < c->block_count);
	  ld_spi_flash_erase((U8)((int)c->context),block * c->block_size,c->block_size);
    return 0;
}

int user_provided_block_device_sync(const struct lfs_config *c) {
    return 0;
}


// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,

    // block device configuration
    .read_size = SPI_FLASH_READ_SIZE,
    .prog_size = SPI_FLASH_PROG_SIZE,
    .block_size = 1024*BLOCK_SIZE,
    .block_count = SPI_FLASH_FULL_SIZE/BLOCK_SIZE,
    .cache_size = SPI_FLASH_CACHE_SIZE,
    .lookahead_size = SPI_FLASH_LOOKAHEAD_SIZE,
	
	  //context就是spi号
	  .context = (void*)SPI_FLASH_PORT, 
};

int lfs_chip_erase(void)
{
	return ld_spi_flash_chip_erase((U8)((int)cfg.context));
}
// entry point
int lfs_test(int c,char*name) {
	  
	
    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
			  lfs_chip_erase();
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }
		
		uint32_t boot_count = 0;
		if(c>10000)c=1;
		while(c>0){

			lfs_file_open(&lfs, &file, name, LFS_O_RDWR | LFS_O_CREAT);
			lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

			// update boot count
			boot_count += 1;
			lfs_file_rewind(&lfs, &file);
			lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

			// remember the storage is not updated until the file is closed successfully
			lfs_file_close(&lfs, &file);
			
			c--;
		}

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    mshell_printf("boot_count: %d\n", boot_count);
		
		return 1;
}
int lfs_id(void)
{
	int id = ld_spi_flash_readID(1);
  return id;	
}
int lfs_list(void)
{
	
}

Export_DIR("/lfs",lfs,0);//定义目录
Export(lfs,e,lfs_chip_erase,"全盘擦除");
Export(lfs,t,lfs_test,"测试lfs.");
Export(lfs,i,lfs_id,"读取flash id");


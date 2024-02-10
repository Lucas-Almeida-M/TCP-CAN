
#include "lfs_app.h"
#include "stm32f7xx_hal_flash.h"
#include "stm32f7xx_hal_flash_ex.h"

#define START_FLASH 0x081e7000

lfs_t lfs;
lfs_file_t file;
struct lfs_config littleCfg;


int flash_read(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size){

	uint32_t StartAddress, i;
	uint32_t *Bff;

	StartAddress = START_FLASH + (block*c->block_size) + off;

	Bff = (uint32_t*)buffer;
	for (i = 0 ; i < (size/4) ; i++)
	{
		*Bff = *(__IO uint32_t*)StartAddress;
		StartAddress += 4;
		Bff++;
	}

	return LFS_ERR_OK;
}

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propagated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int flash_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size){
	uint32_t StartAddress, i;
	uint32_t *Bff;

	StartAddress = START_FLASH + (block*c->block_size) + off;
	Bff = (uint32_t*)buffer;

	HAL_FLASH_Unlock();
	for (i = 0 ; i < (size / 4) ; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartAddress, (uint64_t)(*Bff));
		StartAddress += 4;
		Bff++;
	}
	HAL_FLASH_Lock();

	return LFS_ERR_OK;
}

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propagated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int flash_erase(const struct lfs_config *c, lfs_block_t block){
//	FLASH_Erase_Sector(Sector, VoltageRange)
//	uint32_t StartAddress;
//	uint32_t PageErr;
//	FLASH_EraseInitTypeDef EraseInitStruct;
//
//	StartAddress = START_FLASH + (block*c->block_size);
//	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
//	EraseInitStruct.PageAddress = StartAddress;
//	EraseInitStruct.NbPages = 1;
//	EraseInitStruct.Banks = FLASH_BANK_1;
//	HAL_FLASH_Unlock();
//	HAL_FLASHEx_Erase(&EraseInitStruct, &PageErr);
//	HAL_FLASH_Lock();

	return LFS_ERR_OK;
}

// Sync the state of the underlying block device. Negative error codes
// are propagated to the user.
int flash_sync(const struct lfs_config *c){
	return LFS_ERR_OK;
}


void lfs_init(void)
{
	int32_t error;

	littleCfg.read_size = 64;
	littleCfg.prog_size = 64;
	littleCfg.block_size = 1024;
	littleCfg.block_count = 2;
	littleCfg.cache_size = 256;
	littleCfg.lookahead_size = 8;
	littleCfg.block_cycles = 1000;

	littleCfg.read = flash_read;
	littleCfg.prog = flash_prog;
	littleCfg.erase = flash_erase;
	littleCfg.sync = flash_sync;

	error = lfs_mount(&lfs, &littleCfg);
	if (error != LFS_ERR_OK){
		lfs_format(&lfs, &littleCfg);
		error = lfs_mount(&lfs, &littleCfg);
		if (error != LFS_ERR_OK){
			Error_Handler();
		}
	}
}

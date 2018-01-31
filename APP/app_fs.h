/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_fs.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/21
 * Description        : This file contains the software implementation for the
 *                      file system unit
 *******************************************************************************
 * History:
 * 2013/08/22 v1.0      
 *******************************************************************************/

#ifndef _APP_FS_H_
#define _APP_FS_H_

#include "stm32f2xx.h"

#if defined(USE_FATFS_FTR)
#include "ff.h"


void init_fs_task(void);
u8 create_fatfs(u8 drive_num);
u8 fs_write_file(u8 drive_num, u8* path, u8* p_buf, u32 len);
u8 get_file_size(u8 drive_num, u8* path, u32* file_len);
u8 fs_read_file(u8 drive_num, u8* path, u8* p_buf, u32 len);
u8 get_drive_space(u8 drive_num, u32* p_total, u32* p_free);
u8 mount_fatfs(u8 drive_num);
u8 unmount_fatfs(u8 drive_num);
FRESULT scan_files(char* path);


#endif /* USE_FATFS_FTR */

#endif /* _APP_FS_H_ */


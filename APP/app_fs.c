/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_fs.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/21
 * Description        : This file contains the software implementation for the
 *                      file system unit
 *******************************************************************************
 * History:
 * 2013/08/22 v1.0
 *******************************************************************************/

#include "app_fs.h"
#if defined(USE_FATFS_FTR)
#include "trace.h"
#include "spi_flash.h"
#include "ucos_ii.h"
#include <string.h>


#define FATFS_DRIVE_NUM    2

static OS_STK fs_task_stk[FS_TASK_STK_SIZE];

static FATFS  g_fs[FATFS_DRIVE_NUM];
static FIL    g_fp;

static void fs_task(void *p_arg);


/**
  * @param:  drive_num
  * @return: 0 -- success
  *
  */
u8 create_fatfs(u8 drive_num)
{
	FRESULT  ret;

	assert_param(drive_num < FATFS_DRIVE_NUM);
	ret = f_mkfs(drive_num, 1, 512);
	return ret;
}


/**
  * @param:  drive_num
  * @return: 0 -- success
  *
  */
u8 mount_fatfs(u8 drive_num)
{
	FRESULT  ret;

	assert_param(drive_num < FATFS_DRIVE_NUM);
	ret = f_mount(drive_num, &g_fs[drive_num]);
	return ret;
}


/**
  * @param:  drive_num
  * @return: 0 -- success
  *
  */
u8 unmount_fatfs(u8 drive_num)
{
	FRESULT  ret;

	assert_param(drive_num < FATFS_DRIVE_NUM);
	ret = f_mount(drive_num, NULL);
	return ret;
}



/**
  * @param:  drive_num     drive number
  * @param:  path          Pointer to the file name
  * @param:  p_buf         Pointer to the write buffer
  * @param:  len           write data length
  *
  * @return: 0 -- success
  *
  */
u8 fs_write_file(u8 drive_num, u8* path, u8* p_buf, u32 len)
{
	FRESULT  ret;
	u8       buffer[128];
	u32      write_len;

	snprintf((char*)buffer, sizeof buffer, "%d:%s", drive_num, path);
	/* Create destination file on the drive number */
	ret = f_open(&g_fp, (const TCHAR*)buffer, FA_OPEN_ALWAYS | FA_WRITE);
	if (FR_OK != ret)
		return ret;

	/* Move to end of the file to append data */
	ret = f_lseek(&g_fp, f_size(&g_fp));
	if (FR_OK != ret)
		return ret;

	ret = f_write(&g_fp, p_buf, len, &write_len);
	if (FR_OK != ret)
		return ret;

	if (write_len != len)
		return 1;

	// write success and close file handle
	ret = f_close(&g_fp);
	if (FR_OK != ret)
		return ret;

	return 0;
}


/**
  * @param:  drive_num     drive number
  * @param:  path          Pointer to the file name
  * @param:  p_buf         Pointer to the read buffer
  * @param:  len           read buffer len
  *
  * @return: 0 -- success
  *
  */
u8 get_file_size(u8 drive_num, u8* path, u32* file_len)
{
	FRESULT  ret;
	u8       buffer[128];

	snprintf((char*)buffer, sizeof buffer, "%d:%s", drive_num, path);
	ret = f_open(&g_fp, (const TCHAR*)buffer, FA_OPEN_EXISTING | FA_READ);
	if (FR_OK != ret)
		return ret;

	*file_len = f_size(&g_fp);

	ret = f_close(&g_fp);
	if (FR_OK != ret)
		return ret;

	return 0;
}



/**
  * @param:  drive_num     drive number
  * @param:  path          Pointer to the file name
  * @param:  p_buf         Pointer to the read buffer
  * @param:  len           read buffer len
  *
  * @return: 0 -- success
  *
  */
u8 fs_read_file(u8 drive_num, u8* path, u8* p_buf, u32 len)
{
	FRESULT  ret;
	u8       buffer[128];
	u32      read_len;

	snprintf((char*)buffer, sizeof buffer, "%d:%s", drive_num, path);
	ret = f_open(&g_fp, (const TCHAR*)buffer, FA_OPEN_EXISTING | FA_READ);
	if (FR_OK != ret)
		return ret;

	ret = f_read(&g_fp, p_buf, len, &read_len);
	if (FR_OK != ret)
		return ret;

	ret = f_close(&g_fp);
	if (FR_OK != ret)
		return ret;

	return 0;
}




/**
  * @param:  drive_num
  * @return: 0 -- success
  *
  */
u8 get_drive_space(u8 drive_num, u32* p_total, u32* p_free)
{
	FRESULT  ret;
	FATFS    *fs;
	DWORD    fre_clust, fre_sect, tot_sect;
	u8       vl_buf[32];

	snprintf((char*)vl_buf, sizeof vl_buf, "%d:", drive_num);
	/* Get volume information and free clusters of drive number */
	ret = f_getfree((const TCHAR*)vl_buf, &fre_clust, &fs);
	if (FR_OK != ret)
		return ret;

	/* Get total sectors and free sectors */
	tot_sect = (fs->n_fatent - 2) * fs->csize;
	fre_sect = fre_clust * fs->csize;

	*p_total = tot_sect / 2;
	*p_free  = fre_sect / 2;

	return 0;
}



FRESULT scan_files (
    char* path        /* Start node to be scanned (also used as work area) */
)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                APP_TRACE("%s/%s size: %dbytes\r\n", path, fn, fno.fsize);
            }
        }
    }

    return res;
}


void init_fs_task(void)
{
    INT8U os_err;

    /* File System task ******************************************************/
    os_err = OSTaskCreateExt((void (*)(void *)) fs_task,
                            (void		   * ) 0,
                            (OS_STK 	   * )&fs_task_stk[FS_TASK_STK_SIZE - 1],
                            (INT8U			 ) FS_TASK_PRIO,
                            (INT16U 		 ) FS_TASK_PRIO,
                            (OS_STK 	   * )&fs_task_stk[0],
                            (INT32U 		 ) FS_TASK_STK_SIZE,
                            (void		   * ) 0,
                            (INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(FS_TASK_PRIO, (INT8U *)"FATFS", &os_err);
}


static void fs_task(void *p_arg)
{
	u8 ret;

	APP_TRACE("mount SPI FLASH FATFS...");
	ret = mount_fatfs(0);
	if (ret)
	{
		APP_TRACE("error\r\n");
	}
	else
	{
		APP_TRACE("done\r\n");
	}

	while (1)
	{
		OSTimeDlyHMSM(0, 0, 5, 0);
	}

}


#endif















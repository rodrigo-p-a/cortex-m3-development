#ifndef __ATADRIVE_H_
#define __ATADRIVE_H_

#include <string.h>
#include "diskio.h"

DSTATUS ATA_disk_status(void);
DSTATUS ATA_disk_initialize(void);
DRESULT ATA_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT ATA_disk_write(BYTE *buff, DWORD sector, UINT count);
DRESULT ATA_disk_ioctl (BYTE cmd, void *buff);	/* Always add in diskio.c */

/**
 * @}
 */

#endif /* __FSUSB_CFG_H_ */

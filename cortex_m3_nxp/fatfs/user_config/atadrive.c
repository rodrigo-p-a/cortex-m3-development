#include "diskio.h"
#include "atadrive.h"

/* Local variables */
static volatile DSTATUS status = STA_NOINIT;	/* Disk status */

DSTATUS ATA_disk_initialize(void)
{
	return STA_NODISK;
}

DRESULT ATA_disk_ioctl (BYTE cmd, void *buff)
{
	return RES_NOTRDY;
}

DRESULT ATA_disk_read(BYTE *buff, DWORD sector, UINT count)
{
	return RES_NOTRDY;
}

DSTATUS ATA_disk_status(void)
{
	return STA_NODISK;
}

DRESULT ATA_disk_write(BYTE *buff, DWORD sector, UINT count)
{
	return RES_NOTRDY;
}

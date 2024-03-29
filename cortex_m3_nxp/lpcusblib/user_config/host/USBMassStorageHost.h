/*
 * USBMassStorageHost.h
 *
 *  Created on: Feb 10, 2015
 *      Author: marcelo
 */

#ifndef USER_CONFIG_HOST_USBMASSSTORAGEHOST_H_
#define USER_CONFIG_HOST_USBMASSSTORAGEHOST_H_

#include <string.h>
#include "ffconf.h"
#include "diskio.h"
#include "MassStorageClassHost.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Reference to lpcusblib to link USB Pendrive and FatFS */
#include "MassStorageClassHost.h"

/**
 * @ingroup Mass_Storage_Host
 * @{
 */

typedef USB_ClassInfo_MS_Host_t DISK_HANDLE_T;

/**
 * @brief	Enumerate and get the disk connected
 * @param	hDisk	: Handle to the USB Disk data structure
 * @return	1 on success, 0 on failure
 */
int FSUSB_DiskAcquire(DISK_HANDLE_T *hDisk);

/**
 * @brief	Initialize the disk data structure
 * @return	Pointer to the disk data strucuture
 */
DISK_HANDLE_T *FSUSB_DiskInit(void);

/**
 * @brief	Get the number of sectors in the disk
 * @param	hDisk	: Handle to USB disk
 * @return	Number of sectors in the USB disk
 */
uint32_t FSUSB_DiskGetSectorCnt(DISK_HANDLE_T *hDisk);

/**
 * @brief	Get number of bytes in a sector of the disk
 * @param	hDisk	: Handle to the USB Disk
 * @return	Number of bytes in a single sector of the disk
 */
uint32_t FSUSB_DiskGetSectorSz(DISK_HANDLE_T *hDisk);

/**
 * @brief	Reads sectors from USB mass storage disk
 * @param	hDisk		: Handle to the USB disk
 * @param	buff		: Pointer to memory where the data read will be stored
 * @param	secStart	: Starting sector from which data be read
 * @param	numSec		: Number of sectors to be read
 * @return	1 on success and 0 on failure
 */
int FSUSB_DiskReadSectors(DISK_HANDLE_T *hDisk, void *buff, uint32_t secStart, uint32_t numSec);

/**
 * @brief	Write data to USB device sectors
 * @param	hDisk		: Handle to the USB disk
 * @param	buff		: Pointer to memory where the data to be written, be stored
 * @param	secStart	: Starting sector from which data be written
 * @param	numSec		: Number of sectors to write
 * @return	1 on success and 0 on failure
 */
int FSUSB_DiskWriteSectors(DISK_HANDLE_T *hDisk, void *buff, uint32_t secStart, uint32_t numSec);

/**
 * @brief	Wait for the USB device to be inserted and ready
 * @param	hDisk	: Handle to USB Disk
 * @return	1 on success and 0 on failure
 */
int FSUSB_DiskInsertWait(DISK_HANDLE_T *hDisk);

/**
 * @def		FSMCI_InitRealTimeClock()
 * @brief	Initialize the real time clock
 */
int FSUSB_InitRealTimeClock(void);

/**
 * @brief	Wait for the USB Disk to be ready
 * @param	hDisk	: Pointer to Disk Handle
 * @param	tout	: Time to wait, in milliseconds
 * @return	0 when operation failed 1 when successfully completed
 */
int FSUSB_DiskReadyWait(DISK_HANDLE_T *hDisk, int tout);

void usbHostPendrive(void *pvParameters);
void USB_ReadWriteFile(void);

/* Erase block size fixed to 4K */
#define FSUSB_DiskGetBlockSz(hDisk)         (4 * 1024)

/**
 * @}
 */

#endif /* USER_CONFIG_HOST_USBMASSSTORAGEHOST_H_ */

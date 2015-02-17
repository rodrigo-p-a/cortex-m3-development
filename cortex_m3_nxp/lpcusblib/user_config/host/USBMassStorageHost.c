/*
 * USBMassStorageHost.c
 *
 *  Created on: Feb 10, 2015
 *      Author: marcelo
 */

#include "USB.h"
#include "USBMassStorageHost.h"
#include "MassStorageClassHost.h"

#include "usbdisk.h"

/* File System and HAL */
#include "ff.h"
#include "rtc.h"
#include "uart.h"

/** LPCUSBlib Mass Storage Class driver interface configuration and state information. This structure is
 *  passed to all Mass Storage Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
static USB_ClassInfo_MS_Host_t FlashDisk_MS_Interface = {
	.Config = {
		.DataINPipeNumber       = 1,
		.DataINPipeDoubleBank   = false,

		.DataOUTPipeNumber      = 2,
		.DataOUTPipeDoubleBank  = false,
		.PortNumber = 0,
	},
};

static SCSI_Capacity_t DiskCapacity;
static uint8_t buffer[8 * 1024];

static FATFS fatFS;	/* File system object */
static FIL fileObj;	/* File object */


void usbHostPendrive(void *pvParameters)
{
	USB_CurrentMode = USB_MODE_Host;
	USB_Disable();
	USB_Init();

	UARTSendStr(0, "Mass Storage Host Demo running.\r\n");

	USB_ReadWriteFile();

	UARTSendStr(0, "Example completed.\r\n");

	while (1);
}

/* Function to spin forever when there is an error */
static void die(FRESULT rc)
{
	UARTSendStr(0, "*******DIE %d*******\r\n");
	while (1);/* Spin for ever */
}

/* Function to do the read/write to USB Disk */
void USB_ReadWriteFile(void)
{
	FRESULT rc;		/* Result code */
	int i;
	UINT bw, br;
	uint8_t *ptr;
	char debugBuf[64];
	DIR dir;		/* Directory object */
	FILINFO fno;	/* File information object */

	rc = f_mount(&fatFS, "2:", 1);

	rc = f_open(&fileObj, "2:01u.TXT", FA_READ);
	if (rc) {
		UARTSendStr(0, "Unable to open 01u.TXT from USB Disk\r\n");
	}
	else {
		UARTSendStr(0, "Opened file 01u.TXT from USB Disk. Printing contents...\r\n\r\n");
		for (;; ) {
			/* Read a chunk of file */
			rc = f_read(&fileObj, buffer, sizeof buffer, &br);
			if (rc || !br) {
				break;					/* Error or end of file */
			}
			ptr = (uint8_t *) buffer;
			for (i = 0; i < br; i++) {	/* Type the data */
				printf("%c", ptr[i]);
			}
		}
		if (rc) {
			die(rc);
		}

		UARTSendStr(0, "\r\n\r\nClose the file.\r\n");
		rc = f_close(&fileObj);
		if (rc) {
			die(rc);
		}
	}

	UARTSendStr(0, "\r\nCreate a new file (hello.txt).\r\n");
	rc = f_open(&fileObj, "2:HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) {
		die(rc);
	}
	else {

		UARTSendStr(0, "\r\nWrite a text data. (Hello world!)\r\n");

		rc = f_write(&fileObj, "Hello world!\r\n", 14, &bw);
		if (rc) {
			die(rc);
		}
		else {
			sprintf(debugBuf, "%u bytes written.\r\n", bw);
			UARTSendStr(0, debugBuf);
		}
		UARTSendStr(0, "\r\nClose the file.\r\n");
		rc = f_close(&fileObj);
		if (rc) {
			die(rc);
		}
	}
	UARTSendStr(0, "\r\nOpen root directory.\r\n");
	rc = f_opendir(&dir, "2:");
	if (rc) {
		die(rc);
	}
	else {
		UARTSendStr(0, "\r\nDirectory listing...\r\n");
		for (;; ) {
			/* Read a directory item */
			rc = f_readdir(&dir, &fno);
			if (rc || !fno.fname[0]) {
				break;					/* Error or end of dir */
			}
			if (fno.fattrib & AM_DIR) {
				sprintf(debugBuf, "   <dir>  %s\r\n", fno.fname);
			}
			else {
				sprintf(debugBuf, "   %8lu  %s\r\n", fno.fsize, fno.fname);
			}
			UARTSendStr(0, debugBuf);
		}
		if (rc) {
			die(rc);
		}
	}
	UARTSendStr(0, "\r\nTest completed.\r\n");
	USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);

	while(1);

}

/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(const uint8_t corenum)
{
	printf(("Device Attached on port %d\r\n"), corenum);
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum)
{
	printf(("\r\nDevice Unattached on port %d\r\n"), corenum);
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
	uint16_t ConfigDescriptorSize;
	uint8_t  ConfigDescriptorData[512];
	uint8_t text[128];

	if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize, ConfigDescriptorData,
										   sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful) {
		UARTSendStr(0, "Error Retrieving Configuration Descriptor.\r\n");
		return;
	}

	FlashDisk_MS_Interface.Config.PortNumber = corenum;
	if (MS_Host_ConfigurePipes(&FlashDisk_MS_Interface, ConfigDescriptorSize, ConfigDescriptorData) != MS_ENUMERROR_NoError) {
		UARTSendStr(0, "Attached Device Not a Valid Mass Storage Device.\r\n");
		return;
	}

	if (USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 1) != HOST_SENDCONTROL_Successful) {
		UARTSendStr(0, "Error Setting Device Configuration.\r\n");
		return;
	}

	uint8_t MaxLUNIndex;
	if (MS_Host_GetMaxLUN(&FlashDisk_MS_Interface, &MaxLUNIndex)) {
		UARTSendStr(0, "Error retrieving max LUN index.\r\n");
		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
		return;
	}

	sprintf(text, "Total LUNs: %d - Using first LUN in device.\r\n", (MaxLUNIndex + 1));
	UARTSendStr(0, text);

	if (MS_Host_ResetMSInterface(&FlashDisk_MS_Interface)) {
		UARTSendStr(0, "Error resetting Mass Storage interface.\r\n");
		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
		return;
	}

	SCSI_Request_Sense_Response_t SenseData;
	if (MS_Host_RequestSense(&FlashDisk_MS_Interface, 0, &SenseData) != 0) {
		UARTSendStr(0, "Error retrieving device sense.\r\n");
		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
		return;
	}

	if (MS_Host_PreventAllowMediumRemoval(&FlashDisk_MS_Interface, 0, true)) {
		UARTSendStr(0, "Error setting Prevent Device Removal bit.\r\n");
	    USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
	    return;
	}

	SCSI_Inquiry_Response_t InquiryData;
	if (MS_Host_GetInquiryData(&FlashDisk_MS_Interface, 0, &InquiryData)) {
		UARTSendStr(0, "Error retrieving device Inquiry data.\r\n");
		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
		return;
	}

	sprintf(text, "Vendor \"%.8s\"\r\nProduct \"%.16s\"\r\nRevision \"%.4s\"\r\n", InquiryData.VendorID, InquiryData.ProductID, InquiryData.RevisionID);
	UARTSendStr(0, text);

	UARTSendStr(0, "Mass Storage Device Enumerated.\r\n");
}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t corenum, const uint8_t ErrorCode)
{
	USB_Disable();

	printf(("Host Mode Error\r\n"
			  " -- Error port %d\r\n"
			  " -- Error Code %d\r\n" ), corenum, ErrorCode);

	for (;; ) {}
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t corenum,
											const uint8_t ErrorCode,
											const uint8_t SubErrorCode)
{
	printf(("Dev Enum Error\r\n"
			  " -- Error port %d\r\n"
			  " -- Error Code %d\r\n"
			  " -- Sub Error Code %d\r\n"
			  " -- In State %d\r\n" ),
			 corenum, ErrorCode, SubErrorCode, USB_HostState[corenum]);
}

/**
 * Dummy callback function for DeviceStandardReq.c,
 * this way i don't need remove files.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
									const uint8_t wIndex,
									const void * *const DescriptorAddress)
{
	return NO_DESCRIPTOR;
}

/* Get the disk data structure */
DISK_HANDLE_T *FSUSB_DiskInit(void)
{
	return &FlashDisk_MS_Interface;
}

/* Wait for disk to be inserted */
int FSUSB_DiskInsertWait(DISK_HANDLE_T *hDisk)
{
	while (USB_HostState[hDisk->Config.PortNumber] != HOST_STATE_Configured) {
		MS_Host_USBTask(hDisk);
		USB_USBTask();
	}
	return 1;
}

/* Disk acquire function that waits for disk to be ready */
int FSUSB_DiskAcquire(DISK_HANDLE_T *hDisk)
{
	printf("Waiting for ready...");
	for (;; ) {
		uint8_t ErrorCode = MS_Host_TestUnitReady(hDisk, 0);

		if (!(ErrorCode)) {
			break;
		}

		/* Check if an error other than a logical command error (device busy) received */
		if (ErrorCode != MS_ERROR_LOGICAL_CMD_FAILED) {
			printf("Failed\r\n");
			USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
			return 0;
		}
	}
	printf("Done.\r\n");

	if (MS_Host_ReadDeviceCapacity(hDisk, 0, &DiskCapacity)) {
		printf("Error retrieving device capacity.\r\n");
		USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
		return 0;
	}

	printf(("%lu blocks of %lu bytes.\r\n"), DiskCapacity.Blocks, DiskCapacity.BlockSize);
	return 1;
}

/* Get sector count */
uint32_t FSUSB_DiskGetSectorCnt(DISK_HANDLE_T *hDisk)
{
	return DiskCapacity.Blocks;
}

/* Get Block size */
uint32_t FSUSB_DiskGetSectorSz(DISK_HANDLE_T *hDisk)
{
	return DiskCapacity.BlockSize;
}

/* Read sectors */
int FSUSB_DiskReadSectors(DISK_HANDLE_T *hDisk, void *buff, uint32_t secStart, uint32_t numSec)
{
	if (MS_Host_ReadDeviceBlocks(hDisk, 0, secStart, numSec, DiskCapacity.BlockSize, buff)) {
		printf("Error reading device block.\r\n");
		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
		return 0;
	}
	return 1;
}

/* Write Sectors */
int FSUSB_DiskWriteSectors(DISK_HANDLE_T *hDisk, void *buff, uint32_t secStart, uint32_t numSec)
{
	if (MS_Host_WriteDeviceBlocks(hDisk, 0, secStart, numSec, DiskCapacity.BlockSize, buff)) {
		printf("Error writing device block.\r\n");
		return 0;
	}
	return 1;
}

/* Disk ready function */
int FSUSB_DiskReadyWait(DISK_HANDLE_T *hDisk, int tout)
{
	volatile int i = tout * 100;
	while (i--) {	/* Just delay */
	}
	return 1;
}

int FSUSB_InitRealTimeClock(void)
{
	rtctime inittime;

	RTCInit();

	/* Set current time for RTC */
	/* Current time is 8:00:00PM, 2015-01-01 */
	inittime.rtc_sec = 0;
	inittime.rtc_min = 0;
	inittime.rtc_hour = 20;
	inittime.rtc_yday = 1;
	inittime.rtc_mon = 1;
	inittime.rtc_year = 2015;
	RTCSetTime(inittime);		/* Set local time */

	/* Set current time for RTC */
	/* Current time is 8:00:00PM, 2015-01-01 */
	inittime.rtc_sec = 0;
	inittime.rtc_min = 0;
	inittime.rtc_hour = 20;
	inittime.rtc_yday = 1;
	inittime.rtc_mon = 1;
	inittime.rtc_year = 2014;
	RTCSetAlarm(inittime);		/* Set local time */

	NVIC_EnableIRQ(RTC_IRQn);

	/* mask off alarm mask, turn on IMYEAR in the counter increment interrupt
	register */
	RTCSetAlarmMask(AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR);
	LPC_RTC->CIIR = IMMIN | IMYEAR;
	/* 2007/01/01/00:00:00 is the alarm on */

	RTCStart();

	return 1;
}


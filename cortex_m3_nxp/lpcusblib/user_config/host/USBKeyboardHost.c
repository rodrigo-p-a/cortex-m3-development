#include "USBKeyboardHost.h"
#include "uart.h"


/** LPCUSBlib HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
static USB_ClassInfo_HID_Host_t Keyboard_HID_Interface = {
	.Config = {
		.DataINPipeNumber       = 1,
		.DataINPipeDoubleBank   = false,

		.DataOUTPipeNumber      = 2,
		.DataOUTPipeDoubleBank  = false,

		.HIDInterfaceProtocol   = HID_CSCP_KeyboardBootProtocol,
		.PortNumber = 0,
	},
};

void usbHostKeyboard(void *pvParameters)
{
	USB_CurrentMode = USB_MODE_Host;
	USB_Disable();
	USB_Init();

	UARTSendStr(0, "Keyboard Host Demo running.\r\n");

	while (1)
	{
		KeyboardHost_Task();
		HID_Host_USBTask(&Keyboard_HID_Interface);
		USB_USBTask();
	}
}

/* Keyboard management task */
void KeyboardHost_Task(void)
{
	uint8_t KeyCode;

	if (USB_HostState[Keyboard_HID_Interface.Config.PortNumber] != HOST_STATE_Configured)
	{
		return;
	}

	if (HID_Host_IsReportReceived(&Keyboard_HID_Interface))
	{
		USB_KeyboardReport_Data_t KeyboardReport;
		HID_Host_ReceiveReport(&Keyboard_HID_Interface, &KeyboardReport);

		KeyCode = KeyboardReport.KeyCode[0];

		if (KeyCode)
		{
			char PressedKey = 0;

			/* Retrieve pressed key character if alphanumeric */
			if ((KeyCode >= HID_KEYBOARD_SC_A) && (KeyCode <= HID_KEYBOARD_SC_Z))
			{
				PressedKey = (KeyCode - HID_KEYBOARD_SC_A) + 'a';
			}
			else if ((KeyCode >= HID_KEYBOARD_SC_1_AND_EXCLAMATION) &
					 (KeyCode  < HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS))
			{
				PressedKey = (KeyCode - HID_KEYBOARD_SC_1_AND_EXCLAMATION) + '1';
			}
			else if (KeyCode == HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS)
			{
				PressedKey = '0';
			}
			else if (KeyCode == HID_KEYBOARD_SC_SPACE)
			{
				PressedKey = ' ';
			}
			else if (KeyCode == HID_KEYBOARD_SC_ENTER)
			{
				PressedKey = '\n';
			}

			if (PressedKey) {
				UARTPutChar(0, PressedKey);
			}
		}
	}
}

/* This indicates that a device has been attached to the host,
   and starts the library USB task to begin the enumeration and USB
   management process. */
void EVENT_USB_Host_DeviceAttached(const uint8_t corenum)
{
	UARTSendStr(0, "Device Attached on USB port\r\n");
}

/* This indicates that a device has been removed from the host,
   and stops the library USB task management process. */
void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum)
{
	UARTSendStr(0, "\r\nDevice Unattached on USB port\r\n");
}

/* This indicates that a device has been successfully
   enumerated by the host and is now ready to be used by the
   application. */
void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
	uint16_t ConfigDescriptorSize;
	uint8_t  ConfigDescriptorData[512];

	if (USB_Host_GetDeviceConfigDescriptor(corenum, 1,
			&ConfigDescriptorSize, ConfigDescriptorData,
			sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful)
	{
		UARTSendStr(0, "Error Retrieving Configuration Descriptor.\r\n");
		return;
	}

	Keyboard_HID_Interface.Config.PortNumber = corenum;
	if (HID_Host_ConfigurePipes(&Keyboard_HID_Interface, ConfigDescriptorSize, ConfigDescriptorData) != HID_ENUMERROR_NoError)
	{
		UARTSendStr(0, "Attached Device Not a Valid Keyboard.\r\n");
		return;
	}

	if (USB_Host_SetDeviceConfiguration(Keyboard_HID_Interface.Config.PortNumber, 1) != HOST_SENDCONTROL_Successful)
	{
		UARTSendStr(0, "Error Setting Device Configuration.\r\n");
		return;
	}

	if (HID_Host_SetBootProtocol(&Keyboard_HID_Interface) != 0) {
		UARTSendStr(0, "Could not Set Boot Protocol Mode.\r\n");

		USB_Host_SetDeviceConfiguration(Keyboard_HID_Interface.Config.PortNumber, 0);
		return;
	}

	UARTSendStr(0, "Keyboard Enumerated.\r\n");
}

/* This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t corenum, const uint8_t ErrorCode)
{
	USB_Disable();

	printf(("Host Mode Error\r\n"
			  " -- Error port %d\r\n"
			  " -- Error Code %d\r\n" ), corenum, ErrorCode);

	/* Wait forever */
	for (;;) {}
}

/* This indicates that a problem occurred while enumerating an
   attached USB device. */
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

/* Dummy callback function for HID Parser */
bool CALLBACK_HIDParser_FilterHIDReportItem(HID_ReportItem_t *const CurrentItem)
{
	return true;
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
	// De
//	const uint8_t  DescriptorType   = (wValue >> 8);
//	const uint8_t  DescriptorNumber = (wValue & 0xFF);
//
//	const void *Address = NULL;
//	uint16_t    Size    = NO_DESCRIPTOR;
//
//	switch (DescriptorType) {
//	case DTYPE_Device:
//		Address = &DeviceDescriptor;
//		Size    = sizeof(USB_Descriptor_Device_t);
//		break;
//
//	case DTYPE_Configuration:
//		Address = &ConfigurationDescriptor;
//		Size    = sizeof(USB_Descriptor_Configuration_t);
//		break;
//
//	case DTYPE_String:
//		switch (DescriptorNumber) {
//		case 0x00:
//			Address = LanguageStringPtr;
//			Size    = pgm_read_byte(&LanguageStringPtr->Header.Size);
//			break;
//
//		case 0x01:
//			Address = ManufacturerStringPtr;
//			Size    = pgm_read_byte(&ManufacturerStringPtr->Header.Size);
//			break;
//
//		case 0x02:
//			Address = ProductStringPtr;
//			Size    = pgm_read_byte(&ProductStringPtr->Header.Size);
//			break;
//		}
//
//		break;
//	}
//
//	*DescriptorAddress = Address;
//	return Size;
}

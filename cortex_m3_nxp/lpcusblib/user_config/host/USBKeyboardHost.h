/*
 * USBKeyboardHost.h
 *
 *  Created on: Feb 7, 2015
 *      Author: marcelo
 */

#ifndef USER_CONFIG_USBKEYBOARDHOST_H_
#define USER_CONFIG_USBKEYBOARDHOST_H_

#include "LPC17xx.h"
#include "USB.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

void usbHostKeyboard(void *pvParameters);
void KeyboardHost_Task(void);

#endif /* USER_CONFIG_USBKEYBOARDHOST_H_ */

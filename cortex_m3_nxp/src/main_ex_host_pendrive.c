/*
===============================================================================
 Name        : cortex_m3_nxp.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
	#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "uart.h"

#include "USBMassStorageHost.h"

void blinkLed(void *pvParameters)
{
    LPC_GPIO0->FIODIR |= (1<<4);

    while(1)
    {
        LPC_GPIO0->FIOSET = (1<<4);
        vTaskDelay(500/portTICK_RATE_MS);
        LPC_GPIO0->FIOCLR = (1<<4);
        vTaskDelay(500/portTICK_RATE_MS);
    }
}

int main(void)
{
	SystemCoreClockUpdate();

	/* Initialize UART and Set UART port */
	LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 | (1<<4));
	LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 | (1<<6));
	UARTInit(0, 115200);

	/* create task to blink led */
	xTaskCreate(blinkLed, "ledact", (configMINIMAL_STACK_SIZE / 4), NULL, tskIDLE_PRIORITY, NULL);

	/* Create task to control Pendrive */
	xTaskCreate(usbHostPendrive, "usb", (configMINIMAL_STACK_SIZE * 5), NULL, tskIDLE_PRIORITY, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1);

    return 0 ;
}

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

#include "cr_section_macros.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "ff.h"
#include "uart.h"

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

void sdTimerSupport(void *pvParameters)
{
	while(1)
	{
		disk_timerproc();
		vTaskDelay(10/portTICK_RATE_MS);
	}
}

void sdMount(void *pvParameters)
{
	FATFS FatFs;   /* Work area (file system object) for logical drive */
	FIL fil;       /* File object */
	char line[82]; /* Line buffer */
	FRESULT fr;    /* FatFs return code */

	/* Test Uart...*/
    UARTSendStr(0, "Montando...\r\n");

	/* Register work area to the default drive */
	fr = f_mount(&FatFs, "", 1);

	UARTSendStr(0, "Abrindo...\r\n");

	/* Open a text file */
	fr = f_open(&fil, "message.txt", FA_READ);
	if (fr)
		UARTSendStr(0, "Erro..\r\n");

	/* Read all lines and display it */
	while (f_gets(line, sizeof line, &fil))
		UARTSendStr(0, line);

	UARTSendStr(0, "Final!\r\n");

	/* Close the file */
	f_close(&fil);

	while(1);
}

int main(void) {

	SystemCoreClockUpdate();

	/* Initialize UART and Set UART port */
	LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 | (1<<4));
	LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 | (1<<6));
	UARTInit(0, 115200);

	/* create task to blink led */
	xTaskCreate(blinkLed, "ledact", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

	/* Create task to control SD timer */
	xTaskCreate(sdTimerSupport, "tsdsup", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

	/* Create task to mount and read SDCard */
	xTaskCreate(sdMount, "sdmount", configMINIMAL_STACK_SIZE * 7, NULL, tskIDLE_PRIORITY, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1);

    return 0 ;
}

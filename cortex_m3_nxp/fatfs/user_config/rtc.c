/****************************************************************************
 *   $Id:: rtc.c 5743 2010-11-30 23:18:58Z usb00423                         $
 *   Project: NXP LPC17xx RTC example
 *
 *   Description:
 *     This file contains RTC code example which include RTC initialization,
 *     RTC interrupt handler, and APIs for RTC access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include "LPC17xx.h"
#include "rtc.h"

volatile uint32_t alarm_on = 0;

/*****************************************************************************
** Function name:		RTC_IRQHandler
**
** Descriptions:		RTC interrupt handler, it executes based on the
**						the alarm setting
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTC_IRQHandler (void)
{
  LPC_RTC->ILR |= ILR_RTCCIF;		/* clear interrupt flag */
  alarm_on = 1;
  return;
}

/*****************************************************************************
** Function name:		RTCInit
**
** Descriptions:		Initialize RTC timer
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCInit( void )
{
  alarm_on = 0;

  /* Enable CLOCK into RTC */
  LPC_SC->PCONP |= (1 << 9);

  /* If RTC is stopped, clear STOP bit. */
  if ( LPC_RTC->RTC_AUX & (0x1<<4) )
  {
	LPC_RTC->RTC_AUX |= (0x1<<4);
  }

  /*--- Initialize registers ---*/
  LPC_RTC->AMR = 0;
  LPC_RTC->CIIR = 0;
  LPC_RTC->CCR = 0;
  return;
}

/*****************************************************************************
** Function name:		RTCStart
**
** Descriptions:		Start RTC timer
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCStart( void )
{
  /*--- Start RTC counters ---*/
  LPC_RTC->CCR |= CCR_CLKEN;
  LPC_RTC->ILR = ILR_RTCCIF;
  return;
}

/*****************************************************************************
** Function name:		RTCStop
**
** Descriptions:		Stop RTC timer
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCStop( void )
{
  /*--- Stop RTC counters ---*/
  LPC_RTC->CCR &= ~CCR_CLKEN;
  return;
}

/*****************************************************************************
** Function name:		RTC_CTCReset
**
** Descriptions:		Reset RTC clock tick counter
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTC_CTCReset( void )
{
  /*--- Reset CTC ---*/
  LPC_RTC->CCR |= CCR_CTCRST;
  return;
}

/*****************************************************************************
** Function name:		RTCSetTime
**
** Descriptions:		Setup RTC timer value
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCSetTime( rtctime time )
{
  LPC_RTC->SEC = time.rtc_sec;
  LPC_RTC->MIN = time.rtc_min;
  LPC_RTC->HOUR = time.rtc_hour;
  LPC_RTC->DOM = time.rtc_mday;
  LPC_RTC->DOW = time.rtc_wday;
  LPC_RTC->DOY = time.rtc_yday;
  LPC_RTC->MONTH = time.rtc_mon;
  LPC_RTC->YEAR = time.rtc_year;
  return;
}

/*****************************************************************************
** Function name:		RTCSetAlarm
**
** Descriptions:		Initialize RTC timer
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCSetAlarm(rtctime alarm)
{
  LPC_RTC->ALSEC = alarm.rtc_sec;
  LPC_RTC->ALMIN = alarm.rtc_min;
  LPC_RTC->ALHOUR = alarm.rtc_hour;
  LPC_RTC->ALDOM = alarm.rtc_mday;
  LPC_RTC->ALDOW = alarm.rtc_wday;
  LPC_RTC->ALDOY = alarm.rtc_yday;
  LPC_RTC->ALMON = alarm.rtc_mon;
  LPC_RTC->ALYEAR = alarm.rtc_year;
  return;
}

/*****************************************************************************
** Function name:		RTCGetTime
**
** Descriptions:		Get RTC timer value
**
** parameters:			None
** Returned value:		The data structure of the RTC time table
**
*****************************************************************************/
rtctime RTCGetTime( void )
{
  rtctime localtime;

  localtime.rtc_sec = LPC_RTC->SEC;
  localtime.rtc_min = LPC_RTC->MIN;
  localtime.rtc_hour = LPC_RTC->HOUR;
  localtime.rtc_mday = LPC_RTC->DOM;
  localtime.rtc_wday = LPC_RTC->DOW;
  localtime.rtc_yday = LPC_RTC->DOY;
  localtime.rtc_mon = LPC_RTC->MONTH;
  localtime.rtc_year = LPC_RTC->YEAR;
  return ( localtime );
}

/*****************************************************************************
** Function name:		RTCSetAlarmMask
**
** Descriptions:		Set RTC timer alarm mask
**
** parameters:			Alarm mask setting
** Returned value:		None
**
*****************************************************************************/
void RTCSetAlarmMask( uint32_t AlarmMask )
{
  /*--- Set alarm mask ---*/
  LPC_RTC->AMR = AlarmMask;
  return;
}

/*****************************************************************************
**                            End Of File
******************************************************************************/

/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <utils/Log.h>
#include "main.h"

#define LED_RED_PATH "/sys/class/leds/red/brightness"
#define LED_GREEN_PATH "/sys/class/leds/green/brightness"

#define BREATHLED_COLOR_PATH "/sys/devices/virtual/yyd/breathleds/color"
#define BREATHLED_ONOFF_PATH "/sys/devices/virtual/yyd/breathleds/onoff"
#define BREATHLED_FRQUENCY_PATH "/sys/devices/virtual/yyd/breathleds/frequency"

static int led_green_isOn = CONTROL_UNKNOWN;
static int led_red_isOn = CONTROL_UNKNOWN;
static int nChargingFull = CONTROL_UNKNOWN;
static int nChargingRemoved = CONTROL_UNKNOWN;

pthread_mutex_t lights_mutex;

int setchg_breathled_frequency(char *led, char *clr)		//daviekuo
{
	char ctl[3] = {0};
	if(!strcmp(led, "ear"))
	{
		ctl[0] = 'E';
	}
	else if(!strcmp(led, "chest"))
	{
		ctl[0] = 'C';	
	}

	if(!strcmp(clr, "Const"))
	{
		ctl[1] = 'c';	
	}
	else if(!strcmp(clr, "Low"))
	{
		ctl[1] = 'l';	
	}
	else if(!strcmp(clr, "High"))
	{
		ctl[1] = 'h';	
	}
	else if(!strcmp(clr, "Might"))
	{
		ctl[1] = 'm';	
	}
	ctl[2] = '\0';
	return write_to_file(BREATHLED_FRQUENCY_PATH, ctl, 3);
}

int setchg_breathled_color(char *led, char *clr)		//daviekuo
{
	char ctl[3] = {0};
	if(!strcmp(led, "ear"))
	{
		ctl[0] = 'E';
	}
	else if(!strcmp(led, "chest"))
	{
		ctl[0] = 'C';	
	}

	if(!strcmp(clr, "Red"))
	{
		ctl[1] = 'R';	
	}
	else if(!strcmp(clr, "Green"))
	{
		ctl[1] = 'G';	
	}
	else if(!strcmp(clr, "Yellow"))
	{
		ctl[1] = 'X';	
	}
	ctl[2] = '\0';
	return write_to_file(BREATHLED_COLOR_PATH, ctl, 3);
}
int setchg_breathled_onoff(char *led, char *sta)		//daviekuo
{
	char ctl[3] = {0};
	if(!strcmp(led, "ear"))
	{
		ctl[0] = 'E';
	}
	else if(!strcmp(led, "chest"))
	{
		ctl[0] = 'C';	
	}

	if(!strcmp(sta, "on"))
	{
		ctl[1] = '1';	
	}
	else if(!strcmp(sta, "off"))
	{
		ctl[1] = '0';	
	}
	
	ctl[2] = '\0';
	return write_to_file(BREATHLED_ONOFF_PATH, ctl, 3);
}


void start_red_led(int skew)
{
	if (led_red_isOn == CONTROL_ON)
		return;
	led_red_isOn = CONTROL_ON;

	if (skew > 255) skew = 255;
	else if (skew < 0) skew = 0;
	
	set_int_value(LED_RED_PATH, skew);
}

void stop_red_led()
{
	if (led_red_isOn == CONTROL_OFF)
		return;
	led_red_isOn = CONTROL_OFF;
	set_int_value(LED_RED_PATH, 0);
}

void start_green_led(int skew)
{
	if (led_green_isOn == CONTROL_ON)
		return;
	led_green_isOn = CONTROL_ON;

	if (skew > 255) skew = 255;
	else if (skew < 0) skew = 0;
	
	set_int_value(LED_GREEN_PATH, skew);
}

void stop_green_led()
{
	if (led_green_isOn == CONTROL_OFF)
		return;
	led_green_isOn = CONTROL_OFF;

	set_int_value(LED_GREEN_PATH, 0);
}

int lights_chgfull()
{
	/* do not block this function like sleep */
	KPOC_LOGI("lights_chgfull");

	pthread_mutex_lock(&lights_mutex);
	nChargingFull = CONTROL_ON;
	nChargingRemoved = CONTROL_OFF;

	stop_red_led();
	start_green_led(255);
	pthread_mutex_unlock(&lights_mutex);
	
	return 0;
}

int lights_chgon()
{
	KPOC_LOGI("lights_chgon");

	pthread_mutex_lock(&lights_mutex);
	nChargingFull = CONTROL_OFF;
	nChargingRemoved = CONTROL_OFF;

	stop_green_led();
	start_red_led(255);
	pthread_mutex_unlock(&lights_mutex);
	return 0;
}

int lights_chgexit()
{
	/* do not block this function like sleep */
	KPOC_LOGI("lights_chgexit");

	pthread_mutex_lock(&lights_mutex);
	nChargingFull = CONTROL_OFF;
	nChargingRemoved = CONTROL_ON;

	stop_green_led();
	stop_red_led();
	pthread_mutex_unlock(&lights_mutex);

	return 0;
}


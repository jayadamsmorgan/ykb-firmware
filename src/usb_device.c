/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    USB_Device/HID_Standalone/USB_Device/App/usb_device.c
 * @author  MCD Application Team
 * @brief   This file implements the USB Device
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019-2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_device.h"
#include "crs.h"
#include "error_handler.h"
#include "hal/crs.h"
#include "hal/systick.h"
#include "stm32wbxx_hal_gpio.h"
#include "stm32wbxx_usb_dongle.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_hid.h"

/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
__IO uint32_t remotewakeupon = 0;
extern PCD_HandleTypeDef hpcd_USB_FS;
#define CURSOR_STEP 5
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
extern void SystemClockConfig_Resume(void);
void USBD_Clock_Config(void);
/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef HID_Desc;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/**
 * @brief  Gets Pointer Data.
 * @param  pbuf: Pointer to report
 * @retval None
 */

/**
 * @brief  GPIO EXTI Callback function
 *         Handle remote-wakeup through key button
 * @param  GPIO_Pin
 * @retval None
 */

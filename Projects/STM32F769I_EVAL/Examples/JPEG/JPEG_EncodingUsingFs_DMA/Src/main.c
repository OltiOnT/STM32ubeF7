/**
  ******************************************************************************
  * @file    JPEG/JPEG_EncodingUsingFs_DMA/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use the HW JPEG to Encode a JPEG file with DMA method.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup JPEG_EncodingUsingFs_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FATFS SDFatFs;  /* File system object for SD card logical drive */
char SDPath[4]; /* SD card logical drive path */
FIL JPEG_File;  /* File object */
FIL BMP_File;   /* File object */

JPEG_HandleTypeDef    JPEG_Handle;


/* Private function prototypes -----------------------------------------------*/
static void MPU_Config(void);
static void SystemClock_Config(void);
static void LCD_BriefDisplay(void);
static void LCD_FileErrorDisplay(void);
static void LCD_SizesErrorDisplay(void);
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  uint32_t JpegEncodeProcessing_End = 0;

  /* Configure the MPU attributes */
  MPU_Config();

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();

  /* STM32F7xx HAL library initialization:
       - Configure the Flash ART accelerator on ITCM interface
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 200 MHz */
  SystemClock_Config();

  /* Initialize the LED3 (Red LED , set to On when error) */
  BSP_LED_Init(LED3);

  /* Initialize the LED1 (GREEN LED , set to On when Test OK) */
  BSP_LED_Init(LED1);

  /*##-1- JPEG Initialization ################################################*/
  /* Init The JPEG Color Look Up Tables used for YCbCr to RGB conversion   */
  JPEG_InitColorTables();

   /* Init the HAL JPEG driver */
  JPEG_Handle.Instance = JPEG;
  HAL_JPEG_Init(&JPEG_Handle);

  /*##-2- LCD Configuration ##################################################*/
  /* Initialize the LCD   */
  BSP_LCD_Init();

  BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER);
  BSP_LCD_SelectLayer(0);

  /* Display example brief   */
  LCD_BriefDisplay();

  /*##-3- Link the micro SD disk I/O driver ##################################*/
  if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
  {
    /*##-4- Register the file system object to the FatFs module ##############*/
    if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) == FR_OK)
    {
      /*##-5- Open the BMP file with read access #############################*/
      if(f_open(&BMP_File, "image.bmp", FA_READ) == FR_OK)
      {
        /*##-6- Create the JPEG file with write access ########################*/
        if(f_open(&JPEG_File, "image.jpg", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
        {
          /*##-7- JPEG Encoding with DMA (Not Blocking ) Method ################*/
          JPEG_Encode_DMA(&JPEG_Handle, &BMP_File, &JPEG_File);

          /*##-8- Wait till end of JPEG encoding and perform Input/Output Processing in BackGround  #*/
          do
          {
            JPEG_EncodeInputHandler(&JPEG_Handle);
            JpegEncodeProcessing_End = JPEG_EncodeOutputHandler(&JPEG_Handle);

          }while(JpegEncodeProcessing_End == 0);

          /*##-9- Close the JPEG file #######################################*/
          f_close(&JPEG_File);

          BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
          BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
          BSP_LCD_SetFont(&Font16);
          BSP_LCD_DisplayStringAtLine(20, (uint8_t *)" JPEG file encoded and saved on the uSD");
        }
        /*##-10- Close the BMP file ##########################################*/
        f_close(&BMP_File);
      }
      else /* Can't Open BMP file*/
      {
        LCD_FileErrorDisplay();
        OnError_Handler();
      }
    }
  }

  /* Test accomplished successfully */
  BSP_LED_On(LED1);

  /* Infinite loop */
  while (1)
  {
  }
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef  ret = HAL_OK;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 7;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}

/**
  * @brief  Display Example description.
  * @param  None
  * @retval None
  */
static void LCD_BriefDisplay(void)
{
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 177, 800, 112);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_DisplayStringAtLine(9, (uint8_t *)"   JPEG Encoding from uSD Fatfs : DMA Method");
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAtLine(16, (uint8_t *)" This example shows how to Encode (with DMA) and save a JPEG file in uSD");
}

/**
  * @brief  Display File access error message.
  * @param  None
  * @retval None
  */
static void LCD_FileErrorDisplay(void)
{
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAtLine(27, (uint8_t *)"             Unable to open BMP file image.bmp");
  BSP_LCD_DisplayStringAtLine(28, (uint8_t *)" Please check that a bmp file named image.bmp is stored on the uSD1");

}

/**
  * @brief  Display error message when Image Sizes do not meet the requirements.
  * @param  None
  * @retval None
  */
static void LCD_SizesErrorDisplay(void)
{
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAtLine(27, (uint8_t *)"         Input Sizes of image.bmp do not meet requirements");
  BSP_LCD_DisplayStringAtLine(28, (uint8_t *)"    Please change the bmp file named image.bmp stored on the uSD1");

}
/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU as Strongly ordered for not defined regions */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes FMC control registers */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xA0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_8KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  On Error Handler.
  * @param  None
  * @retval None
  */
void OnError_Handler(void)
{
  BSP_LED_On(LED3);
  while(1) { ; } /* Blocking on error */
}

/**
  * @brief  Get the images sizes from BMP header.
  * @param  MyFile: pointer to the source file name
  * @param  pInfo: pointer to the Info structure
  * @retval None
  */
void BMP_GetInfo(FIL * MyFile, JPEG_ConfTypeDef *pInfo)
{
  uint32_t bytesReadfile = 0;
  uint32_t ImageWidth = 0;
  uint32_t ImageHeight = 0 ;
  uint32_t ImageWidthOffset = 18;
  uint32_t ImageHeightOffset = 22;

  /*Open the image with read access */
  f_lseek (MyFile,ImageWidthOffset);
  if(f_read (MyFile, (void *) &ImageWidth , 4, (UINT*)(&bytesReadfile)) == FR_OK)
  {
    pInfo->ImageWidth         = ImageWidth;
  }
  f_lseek (MyFile,ImageHeightOffset);
  if(f_read (MyFile, (void *) &ImageHeight , 4, (UINT*)(&bytesReadfile)) == FR_OK)
  {
    pInfo->ImageHeight        = ImageHeight;
  }

  /* Jpeg Encoding Setting to be set by users */
  pInfo->ChromaSubsampling  = JPEG_CHROMA_SAMPLING;
  pInfo->ColorSpace         = JPEG_COLOR_SPACE;
  pInfo->ImageQuality       = JPEG_IMAGE_QUALITY;

  /*Check if Image Sizes meets the requirements */
  if (((pInfo->ImageWidth % 8) != 0 ) || ((pInfo->ImageHeight % 8) != 0 ) || \
      (((pInfo->ImageWidth % 16) != 0 ) && (pInfo->ColorSpace == JPEG_YCBCR_COLORSPACE) && (pInfo->ChromaSubsampling != JPEG_444_SUBSAMPLING)) || \
      (((pInfo->ImageHeight % 16) != 0 ) && (pInfo->ColorSpace == JPEG_YCBCR_COLORSPACE) && (pInfo->ChromaSubsampling == JPEG_420_SUBSAMPLING)))
  {
    LCD_SizesErrorDisplay();
    OnError_Handler();
  }

}
/**
  * @}
  */

/**
  @page PWR_CurrentConsumption PWR Current Consumption example
  
  @verbatim
  ******************************************************************************
  * @file    PWR/PWR_CurrentConsumption/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR Current Consumption example.
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
  @endverbatim

@par Example Description 

How to configure the system to measure the current consumption in different 
low-power modes.

The Low Power modes are:
  - Sleep Mode
  - STOP mode with RTC
  - STANDBY mode without RTC and BKPSRAM
  - STANDBY mode with RTC
  - STANDBY mode with RTC and BKPSRAM
  
To run this example, user has to follow these steps:
 1. Select the Low power modes to be measured by uncommenting the corresponding
    line inside the stm32f7xx_lp_modes.h file.
    @code
       /* #define SLEEP_MODE               */
       /* #define STOP_MODE                */
       /* #define STANDBY_MODE             */
       /* #define STANDBY_RTC_MODE         */
       /* #define STANDBY_RTC_BKPSRAM_MODE */
    @endcode       

 2. Use an external amperemeter to measure the IDD current. 

 3. This example can not be used in DEBUG mode, this is due to the fact that the 
    Cortex-M7 core is no longer clocked during low power mode so debugging 
    features are disabled.

Here below a detailed description of the example code:

  @verbatim

 1. After reset, the program waits for User button be pressed
     - green LED (LED6) is blinking slowly (1 sec.)
     - to enter the selected low power mode press User button.
     - When the RTC is not used in the low power mode configuration, press
       again the User button to exit the low power mode.
     - When the RTC is used, the wakeup from low power mode is automatically 
       generated by the RTC (after 20s)
     - Green LED (LED6) toggles faster after wake up from Low Power modes..

--> In anyway, wrong end of test is showing red LED (LED5) stay ON

 2. Low power modes description:

    - Sleep Mode
    ============  
            - System Running at PLL (216MHz)
            - Flash 5 wait state
            - Instruction and Data caches ON
            - Prefetch OFF       
            - Code running from Internal FLASH
            - All peripherals disabled
            - Wakeup using EXTI Line (User Button)

    - STOP Mode
    ===========
            - RTC Clocked by LSI
            - Regulator in LP mode
            - HSI, HSE OFF and LSI if not used as RTC Clock source
            - No IWDG
            - FLASH in deep power down mode
            - Automatic Wakeup using RTC clocked by LSI (after ~20s)

    - STANDBY Mode
    ==============
            - Backup SRAM and RTC OFF
            - IWDG and LSI OFF
            - Wakeup using WakeUp Pin (USER Button);
                        
    - STANDBY Mode with RTC clocked by LSI 
    ==========================================
            - RTC Clocked by LSI
            - IWDG OFF and LSI OFF  if not used as RTC Clock source
            - Backup SRAM OFF
            - Automatic Wakeup using RTC clocked by LSI (after ~20s)

  - STANDBY Mode with backup SRAM ON
    ======================================================
            - Backup SRAM ON
            - IWDG OFF
            - Wakeup using WakeUp Pin (USER Button)

   @endverbatim


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.
      
@note On the STM32F23E-Discovery board, an extra current consumption is added due to the LCD. 
      So, to reach the correct current consumption values, unplug LCD flex and touch panel flex.


@par Keywords

Power, STOP, Sleep, Standby, Current Consumption, Low Power, LSI, Backup SRAM, Voltage range

@Note�If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),that is shared between several processors,
 �����then it is highly recommended to enable the CPU cache and maintain its coherence at application level.
������The address and the size of cacheable buffers (shared between CPU and other masters)  must be properly updated to be aligned to cache line size (32 bytes).

@Note It is recommended to enable the cache and maintain its coherence, but depending on the use case
����� It is also possible to configure the MPU as "Write through", to guarantee the write access coherence.
������In that case, the MPU must be configured as Cacheable/Bufferable/Not Shareable.
������Even though the user must manage the cache coherence for read accesses.
������Please refer to the AN4838 �Managing memory protection unit (MPU) in STM32 MCUs�
������Please refer to the AN4839 �Level 1 cache on STM32F7 Series�

@par Directory contents 

  - PWR/PWR_CurrentConsumption/Inc/stm32f7xx_hal_conf.h     HAL configuration file
  - PWR/PWR_CurrentConsumption/Inc/stm32f7xx_it.h           Interrupt handlers header file
  - PWR/PWR_CurrentConsumption/Inc/main.h                   Main program header file 
  - PWR/PWR_CurrentConsumption/Inc/stm32f7xx_lp_modes.h     Low Power Modes header file
  - PWR/PWR_CurrentConsumption/Src/stm32f7xx_it.c           Interrupt handlers
  - PWR/PWR_CurrentConsumption/Src/main.c                   Main program
  - PWR/PWR_CurrentConsumption/Src/stm32f7xx_hal_msp.c      HAL MSP module
  - PWR/PWR_CurrentConsumption/Src/stm32f7xx_lp_modes.c     STM32F7xx Low Power Modes source file
  - PWR/PWR_CurrentConsumption/Src/system_stm32f7xx.c       STM32F7xx system clock configuration file

@par Hardware and Software environment

  - This example runs on STM32F722xx/STM32F723xx/STM32F732xx/STM32F733xx devices.
    
  - This example has been tested with STMicroelectronics STM32F723E-DISCOVERY
    board and can be easily tailored to any other supported device 
    and development board.

  - STM32F723E-DISCOVERY Set-up
    - LED5 (RED) will stay ON if initialization fails.
    - Use LED6.
      * LED6 (GREEN) will slowly toggle (1sec.) waiting for user to launch test, then will turn OFF
      * LED6 (GREEN) will toggle fast (100ms) while returning from STANDBY mode (PWR flag check callback) 
      * LED6 (GREEN) will toggle fast (100ms) at the end of test in case of success.
    - Use User Button.
    - Unsolder R27 resistor (0 ohm) and connect an amperemeter to the pads of this resistor 
      to measure the IDD current or add a jumper on JP1 to perform measurements.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 */

/**
  ******************************************************************************
  * @file    Examples_LL/I2C/I2C_OneBoard_Communication_DMAAndIT/Src/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    14-April-2017
  * @brief   This example describes how to send/receive bytes over I2C IP using
  *          the STM32F1xx I2C LL API.
  *          Peripheral initialization done using LL unitary services functions.
******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F1xx_LL_Examples
  * @{
  */

/** @addtogroup I2C_OneBoard_Communication_DMAAndIT
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/**
  * @brief Timeout value
  */
#if (USE_TIMEOUT == 1)
#define DMA_SEND_TIMEOUT_TC_MS         5
#endif /* USE_TIMEOUT */

/**
  * @brief I2C devices settings
  */
/* I2C SPEEDCLOCK define to max value: 400 KHz */
#define I2C_SPEEDCLOCK                 400000
#define I2C_DUTYCYCLE                  LL_I2C_DUTYCYCLE_2

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#if (USE_TIMEOUT == 1)
uint32_t Timeout                       = 0; /* Variable used for Timeout management */
#endif /* USE_TIMEOUT */
__IO uint8_t ubButtonPress             = 0;

/**
  * @brief Variables related to MasterTransmit process
  */
const uint8_t aLedOn[]                 = "LED ON";
__IO uint8_t  ubNbDataToTransmit       = sizeof(aLedOn);
uint8_t*      pTransmitBuffer          = (uint8_t*)aLedOn;
__IO uint8_t  ubTransferComplete       = 0;

/**
  * @brief Variables related to SlaveReceive process
  */
uint8_t      aReceiveBuffer[0xF]       = {0};
__IO uint8_t ubReceiveIndex            = 0;

/* Private function prototypes -----------------------------------------------*/
void     SystemClock_Config(void);
void     Configure_DMA(void);
void     Configure_I2C_Slave(void);
void     Configure_I2C_Master(void);
void     Activate_I2C_Slave(void);
void     Activate_I2C_Master(void);
uint8_t  Buffercmp8(uint8_t* pBuffer1, uint8_t* pBuffer2, uint8_t BufferLength);
void     LED_Init(void);
void     LED_On(void);
void     LED_Off(void);
void     LED_Blinking(uint32_t Period);
void     WaitForUserButtonPress(void);
void     Handle_I2C_Slave(void);
void     Handle_I2C_Master(void);
void     UserButton_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the system clock to 72 MHz */
  SystemClock_Config();

  /* Initialize LED2 */
  LED_Init();

  /* Set LED2 Off */
  LED_Off();

  /* Initialize User push-button in EXTI mode */
  UserButton_Init();

  /* Configure DMA1_Channel4 (DMA IP configuration in transfer memory to peripheral (I2C2)  */
  Configure_DMA();

  /* Configure I2C1 (I2C IP configuration in Slave mode and related GPIO initialization) */
  Configure_I2C_Slave();

  /* Configure I2C2 (I2C IP configuration in Master mode and related GPIO initialization) */
  Configure_I2C_Master();

  /* Enable the I2C1 peripheral (Slave) */
  Activate_I2C_Slave();

  /* Enable the I2C2 peripheral (Master) */
  Activate_I2C_Master();

  /* Wait for User push-button press to start transfer */
  WaitForUserButtonPress();

  /* Handle I2C1 events (Slave) */
  Handle_I2C_Slave();

  /* Handle I2C2 events (Master) */
  Handle_I2C_Master();

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  This function configures the DMA1_Channel4 to copy data from 
  *         Flash memory(pTransmitBuffer) to I2C2(TXDR).
  * @note   This function is used to :
  *         -1- Enable DMA1 clock
  *         -2- Configure NVIC for DMA1.
  *         -3- Configure the DMA1 functionnal parameters.
  *         -4- Enable DMA1 interrupts complete/error.
  * @param   None
  * @retval  None
  */
void Configure_DMA(void)
{
  /* (1) Enable the clock of DMA1 */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* (2) Configure NVIC for DMA1 */
  NVIC_SetPriority(DMA1_Channel4_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Channel4_IRQn);
	
  /* (3) Configure the DMA1 functionnal parameters */
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_4, LL_DMA_DIRECTION_MEMORY_TO_PERIPH | \
                                                LL_DMA_PRIORITY_HIGH              | \
                                                LL_DMA_MODE_NORMAL                | \
                                                LL_DMA_PERIPH_NOINCREMENT           | \
                                                LL_DMA_MEMORY_INCREMENT           | \
                                                LL_DMA_PDATAALIGN_BYTE            | \
                                                LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, ubNbDataToTransmit);
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, (uint32_t)(pTransmitBuffer), (uint32_t)LL_I2C_DMA_GetRegAddr(I2C2), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4));
  /* (4) Enable DMA1 interrupts complete/error */
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
  LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);
}

/**
  * @brief  This function configures I2C1 in Slave mode.
  * @note   This function is used to :
  *         -1- Enables GPIO clock.
  *         -2- Enable the I2C1 peripheral clock and configures the I2C1 pins.
  *         -3- Configure NVIC for I2C1.
  *         -4- Configure I2C1 functional parameters.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void Configure_I2C_Slave(void)
{
  /* (1) Enables GPIO clock */
  /* Enable the peripheral clock of GPIOB */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

  /* (2) Enable the I2C1 peripheral clock *************************************/

  /* Enable the peripheral clock for I2C1 */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
  
  /* Configure SCL Pin as : Alternate function, High Speed, Open drain, Pull up */
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN);
  LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

  /* Configure SDA Pin as : Alternate function, High Speed, Open drain, Pull up */
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
  LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);
  /* (3) Configure NVIC for I2C1 **********************************************/

  /* Configure Event IT:
   *  - Set priority for I2C1_EV_IRQn
   *  - Enable I2C1_EV_IRQn
   */
  NVIC_SetPriority(I2C1_EV_IRQn, 0);  
  NVIC_EnableIRQ(I2C1_EV_IRQn);

  /* Configure Error IT:
   *  - Set priority for I2C1_ER_IRQn
   *  - Enable I2C1_ER_IRQn
   */
  NVIC_SetPriority(I2C1_ER_IRQn, 0);  
  NVIC_EnableIRQ(I2C1_ER_IRQn);

  /* (4) Configure I2C1 functional parameters ***********************/
  
  /* Disable I2C1 prior modifying configuration registers */
  LL_I2C_Disable(I2C1);
  
  /* Configure the Own Address1 :
   *  - OwnAddress1 is SLAVE_OWN_ADDRESS
   *  - OwnAddrSize is LL_I2C_OWNADDRESS1_7BIT
   */
  LL_I2C_SetOwnAddress1(I2C1, SLAVE_OWN_ADDRESS, LL_I2C_OWNADDRESS1_7BIT);

  /* Enable Clock stretching */
  /* Reset Value is Clock stretching enabled */
  //LL_I2C_EnableClockStretching(I2C1);
  
  /* Enable General Call                  */
  /* Reset Value is General Call disabled */
  //LL_I2C_EnableGeneralCall(I2C1);

  /* Configure the 7bits Own Address2     */
  /* Reset Values of :
   *     - OwnAddress2 is 0x00
   *     - Own Address2 is disabled
   */
  //LL_I2C_SetOwnAddress2(I2C1, 0x00);
  //LL_I2C_DisableOwnAddress2(I2C1);

  /* Enable Peripheral in I2C mode */
  /* Reset Value is I2C mode */
  //LL_I2C_SetMode(I2C1, LL_I2C_MODE_I2C);
}

/**
  * @brief  This function configures I2C2 in Master mode.
  * @note   This function is used to :
  *         -1- Enables GPIO clock.
  *         -2- Enable the I2C2 peripheral clock and configures the I2C2 pins.
  *         -3- Configure NVIC for I2C2.
  *         -4- Configure I2C2 functional parameters.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void Configure_I2C_Master(void)
{
  LL_RCC_ClocksTypeDef rcc_clocks;

  /* (1) Enables GPIO clock  **********************/

  /* Enable the peripheral clock of GPIOB */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

  /* (2) Enable the I2C2 peripheral clock *************************************/

  /* Enable the peripheral clock for I2C2 */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

  /* Configure SCL Pin as : Alternate function, High Speed, Open drain, Pull up */
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_OPENDRAIN);
  LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);

  /* Configure SDA Pin as : Alternate function, High Speed, Open drain, Pull up */
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_11, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_11, LL_GPIO_OUTPUT_OPENDRAIN);
  LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_11, LL_GPIO_PULL_UP);

  /* (3) Configure NVIC for I2C2 **********************************************/

  /* Configure Event IT:
   *  - Set priority for I2C2_EV_IRQn
   *  - Enable I2C2_EV_IRQn
   */
  NVIC_SetPriority(I2C2_EV_IRQn, 0);  
  NVIC_EnableIRQ(I2C2_EV_IRQn);

  /* Configure Error IT:
   *  - Set priority for I2C2_ER_IRQn
   *  - Enable I2C2_ER_IRQn
   */
  NVIC_SetPriority(I2C2_ER_IRQn, 0);  
  NVIC_EnableIRQ(I2C2_ER_IRQn);

  /* (4) Configure I2C2 functional parameters ********************************/
  
  /* Disable I2C2 prior modifying configuration registers */
  LL_I2C_Disable(I2C2);
  
  /* Retrieve Clock frequencies */
  LL_RCC_GetSystemClocksFreq(&rcc_clocks);

  /* Configure the SCL Clock Speed */
  LL_I2C_ConfigSpeed(I2C2, rcc_clocks.PCLK1_Frequency, I2C_SPEEDCLOCK, I2C_DUTYCYCLE);
  
  /* Configure the Own Address1                   */
  /* Reset Values of :
   *     - OwnAddress1 is 0x00
   *     - OwnAddrSize is LL_I2C_OWNADDRESS1_7BIT
   */
  //LL_I2C_SetOwnAddress1(I2C2, 0x00, LL_I2C_OWNADDRESS1_7BIT);

  /* Enable Clock stretching */
  /* Reset Value is Clock stretching enabled */
  //LL_I2C_EnableClockStretching(I2C2);

  
  /* Enable General Call                  */
  /* Reset Value is General Call disabled */
  //LL_I2C_EnableGeneralCall(I2C2);

  /* Configure the 7bits Own Address2     */
  /* Reset Values of :
   *     - OwnAddress2 is 0x00
   *     - Own Address2 is disabled
   */
  //LL_I2C_SetOwnAddress2(I2C2, 0x00);
  //LL_I2C_DisableOwnAddress2(I2C2);

  /* Enable Peripheral in I2C mode */
  /* Reset Value is I2C mode */
  //LL_I2C_SetMode(I2C2, LL_I2C_MODE_I2C);
}

/**
  * @brief  This function Activate I2C1 peripheral (Slave)
  * @note   This function is used to :
  *         -1- Enable I2C1.
  *         -2- Enable I2C1 transfer event/error interrupts.
  * @param  None
  * @retval None
  */
void Activate_I2C_Slave(void)
{
  /* (1) Enable I2C1 **********************************************************/
  LL_I2C_Enable(I2C1);
  
  /* (2) Enable I2C1 transfer event/error interrupts:
   *  - Enable Events interrupts
   *  - Enable Errors interrupts
   */
  LL_I2C_EnableIT_EVT(I2C1);
  LL_I2C_EnableIT_ERR(I2C1);
}

/**
  * @brief  This function Activate I2C2 peripheral (Master)
  * @note   This function is used to :
  *         -1- Enable I2C2.
  *         -2- Enable I2C2 transfer event/error interrupts.
  * @param  None
  * @retval None
  */
void Activate_I2C_Master(void)
{
  /* (1) Enable I2C2 **********************************************************/
  LL_I2C_Enable(I2C2);

  /* (2) Enable I2C2 transfer event/error interrupts:
   *  - Enable Events interrupts
   *  - Enable Errors interrupts
   */
  LL_I2C_EnableIT_EVT(I2C2);
  LL_I2C_EnableIT_ERR(I2C2);
}

/**
  * @brief  Compares two 8-bit buffers and returns the comparison result.
  * @param  pBuffer1: pointer to the source buffer to be compared to.
  * @param  pBuffer2: pointer to the second source buffer to be compared to the first.
  * @param  BufferLength: buffer's length.
  * @retval 0: Comparison is OK (the two Buffers are identical)
  *         Value different from 0: Comparison is NOK (Buffers are different)
  */
uint8_t Buffercmp8(uint8_t* pBuffer1, uint8_t* pBuffer2, uint8_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  Initialize LED2.
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
  /* Enable the LED2 Clock */
  LED2_GPIO_CLK_ENABLE();

  /* Configure IO in output push-pull mode to drive external LED2 */
  LL_GPIO_SetPinMode(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_MODE_OUTPUT);
  /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
  //LL_GPIO_SetPinOutputType(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW */
  //LL_GPIO_SetPinSpeed(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_SPEED_FREQ_LOW);
  /* Reset value is LL_GPIO_PULL_NO */
  //LL_GPIO_SetPinPull(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_PULL_NO);
}

/**
  * @brief  Turn-on LED2.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED2 on */
  LL_GPIO_SetOutputPin(LED2_GPIO_PORT, LED2_PIN);
}

/**
  * @brief  Turn-off LED2.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LED2 off */
  LL_GPIO_ResetOutputPin(LED2_GPIO_PORT, LED2_PIN);
}

/**
  * @brief  Set LED2 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :   
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Turn LED2 on */
  LL_GPIO_SetOutputPin(LED2_GPIO_PORT, LED2_PIN);
  
  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);  
    LL_mDelay(Period);
  }
}

/**
  * @brief  Configures User push-button in GPIO or EXTI Line Mode.
  * @param  None 
  * @retval None
  */
void UserButton_Init(void)
{
  /* Enable the BUTTON Clock */
  USER_BUTTON_GPIO_CLK_ENABLE();

  /* Configure GPIO for BUTTON */
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT);

  /* Connect External Line to the GPIO*/
  USER_BUTTON_SYSCFG_SET_EXTI();

  /* Enable a rising trigger External line 13 Interrupt */
  USER_BUTTON_EXTI_LINE_ENABLE();
  USER_BUTTON_EXTI_FALLING_TRIG_ENABLE();

  /* Configure NVIC for USER_BUTTON_EXTI_IRQn */
  NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn); 
  NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 0x03);  
}

/**
  * @brief  Wait for User push-button press to start transfer.
  * @param  None 
  * @retval None
  */
  /*  */
void WaitForUserButtonPress(void)
{
  while (ubButtonPress == 0)
  {
    LL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
    LL_mDelay(LED_BLINK_FAST);
  }
  /* Turn LED2 off */
  LL_GPIO_ResetOutputPin(LED2_GPIO_PORT, LED2_PIN);
}

/**
  * @brief  This Function handle Slave events to perform a transmission process
  * @note  This function is composed in one step :
  *        -1- Prepare acknowledge for Slave address reception.
  * @param  None
  * @retval None
  */
void Handle_I2C_Slave(void)
{
  /* (1) Prepare acknowledge for Slave address reception **********************/
  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
}

/**
  * @brief  This Function handle Master events to perform a transmission process
  * @note  This function is composed in different steps :
  *        -1- Enable DMA transfer.
  *        -2- Prepare acknowledge for Master data reception.
  *        -3- Initiate a Start condition to the Slave device.
  *        -4- Loop until end of transfer completed (DMA TC raised).
  *        -5- End of tranfer, Data consistency are checking into Slave process.
  * @param  None
  * @retval None
  */
void Handle_I2C_Master(void)
{
  /* (1) Enable DMA transfer **************************************************/
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
  /* (2) Prepare acknowledge for Master data reception ************************/
  LL_I2C_AcknowledgeNextData(I2C2, LL_I2C_ACK);
  
  /* (3) Initiate a Start condition to the Slave device ***********************/
  /* Master Generate Start condition */
  LL_I2C_GenerateStartCondition(I2C2);

  /* (4) Loop until end of transfer completed (DMA TC raised) *****************/

#if (USE_TIMEOUT == 1)
  Timeout = DMA_SEND_TIMEOUT_TC_MS;
#endif /* USE_TIMEOUT */

  /* Loop until DMA transfer complete event */
  while(!ubTransferComplete)
  {
#if (USE_TIMEOUT == 1)
    /* Check Systick counter flag to decrement the time-out value */
    if (LL_SYSTICK_IsActiveCounterFlag()) 
    {
      if(Timeout-- == 0)
      {
        /* Time-out occurred. Set LED to blinking mode */
        LED_Blinking(LED_BLINK_SLOW);
      }
    }
#endif /* USE_TIMEOUT */
  }

  /* (5) End of tranfer, Data consistency are checking into Slave process *****/
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  /* Set FLASH latency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

  /* Enable HSE oscillator */
  LL_RCC_HSE_EnableBypass();
  LL_RCC_HSE_Enable();
  while(LL_RCC_HSE_IsReady() != 1)
  {
  };

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };

  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };

  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 72MHz */
  LL_Init1msTick(72000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(72000000);
}/******************************************************************************/
/*   IRQ HANDLER TREATMENT Functions                                          */
/******************************************************************************/
/**
  * @brief  Function to manage User push-button
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Update User push-button variable : to be checked in waiting loop in main program */
  ubButtonPress = 1;
}

/**
  * @brief  Function called from I2C IRQ Handler when RXNE flag is set
  *         Function is in charge of retrieving received byte on I2C lines.
  * @param  None
  * @retval None
  */
void Slave_Reception_Callback(void)
{
  /* Read character in Receive Data register.
  RXNE flag is cleared by reading data in RXDR register */
  aReceiveBuffer[ubReceiveIndex++] = LL_I2C_ReceiveData8(I2C1);
}

/**
  * @brief  Function called from I2C IRQ Handler when STOP flag is set
  *         Function is in charge of checking data received,
  *         LED2 is On if data are correct.
  * @param  None
  * @retval None
  */
void Slave_Complete_Callback(void)
{
  /* Check if datas request to turn on the LED2 */
  if(Buffercmp8((uint8_t*)aReceiveBuffer, (uint8_t*)aLedOn, (ubReceiveIndex-1)) == 0)
  {
    /* Turn LED2 On:
     *  - Expected bytes have been received
     *  - Slave Rx sequence completed successfully
     */
    LED_On();
  }
  else
  {
    /* Call Error function */
    Error_Callback();
  }
}

/**
  * @brief  DMA transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
void Transfer_Complete_Callback()
{
  /* Generate Stop condition */
  LL_I2C_GenerateStopCondition(I2C2);

  /* DMA transfer completed */
  ubTransferComplete = 1;
}

/**
  * @brief  DMA transfer error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
void Transfer_Error_Callback()
{
  /* Disable DMA1_Channel4_IRQn */
  NVIC_DisableIRQ(DMA1_Channel4_IRQn);
                  
  /* Error detected during DMA transfer */
  LED_Blinking(LED_BLINK_ERROR);
}

/**
  * @brief  Function called in case of error detected in I2C IT Handler
  * @param  None
  * @retval None
  */
void Error_Callback(void)
{
  /* Disable I2C1_EV_IRQn */
  NVIC_DisableIRQ(I2C1_EV_IRQn);

  /* Disable I2C1_ER_IRQn */
  NVIC_DisableIRQ(I2C1_ER_IRQn);

  /* Unexpected event : Set LED2 to Blinking mode to indicate error occurs */
  LED_Blinking(LED_BLINK_ERROR);
}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

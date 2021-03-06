/**  
  ******************************************************************************  
  * File Name          : main.c  
  * Description        : Main program body  
  ******************************************************************************  
  ** This notice applies to any and all portions of this file  
  * that are not between comment pairs USER CODE BEGIN and  
  * USER CODE END. Other portions of this file, whether   
  * inserted by the user or by software development tools  
  * are owned by their respective copyright owners.  
  *  
  * COPYRIGHT(c) 2017 STMicroelectronics  
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
#include "stm32f4xx_hal.h"  
#include "can.h"  
#include "dma.h"  
#include "spi.h"  
#include "tim.h"  
#include "usart.h"  
#include "gpio.h"  
  
/* USER CODE BEGIN Includes */  
#include "chassis_motors.h" 
#include "test_DBUS.h" 
#include "test_drive.h" 
  
/* USER CODE END Includes */  
  
/* Private variables ---------------------------------------------------------*/  
  
/* USER CODE BEGIN PV */  
/* Private variables ---------------------------------------------------------*/ 
extern volatile RC_Ctl_t RC_Ctl; 
/* USER CODE END PV */  
  
typedef enum{
	UP = 1,
	DOWN,
	MID
}buttonPos;	
/* Private function prototypes -----------------------------------------------*/  
void SystemClock_Config(void);  
/* USER CODE BEGIN PFP */  
/* Private function prototypes -----------------------------------------------*/  
 void drive(int RX_X2, int RX_Y1, int RX_X1,int RX_Y2,buttonPos b1, buttonPos b2);
 void GPIO_PP_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
/* USER CODE END PFP */  
  
/* USER CODE BEGIN 0 */  
uint8_t MOTOR_IS_ON = 0;    //ALL MOTOR OFF  
/* USER CODE END 0 */  
  
int main(void)  
{  
  
    /* USER CODE BEGIN 1 */  
  
    /* USER CODE END 1 */  
  
    /* MCU Configuration----------------------------------------------------------*/  
  
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */  
	   HAL_Init();  
  
    /* USER CODE BEGIN Init */  
    //printf("Hello World");  
    /* USER CODE END Init */  
  
    /* Configure the system clock */  
    SystemClock_Config();  
  
    /* USER CODE BEGIN SysInit */  
  
    /* USER CODE END SysInit */  
  
    /* Initialize all configured peripherals */  
    MX_GPIO_Init();  
    MX_DMA_Init();  
    MX_CAN1_Init();  
    MX_USART1_UART_Init();  
    MX_SPI5_Init();  
    MX_USART3_UART_Init();  
    MX_USART6_UART_Init();  
    MX_CAN2_Init();  
    MX_TIM5_Init();  
    MX_TIM2_Init();  
    MX_TIM3_Init();  
    MX_TIM4_Init();  
    MX_TIM8_Init();  
    MX_TIM12_Init();  
    MX_USART2_UART_Init();  
  
    /* USER CODE BEGIN 2 */  
    RC_Init();  
		CanFilter_Init(&hcan1);  
		HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);  
		GPIO_PP_Init(GPIOA, GPIO_PIN_5);
		GPIO_PP_Init(GPIOE, GPIO_PIN_6);
		GPIO_PP_Init(GPIOE, GPIO_PIN_5);
		GPIO_PP_Init(GPIOF, GPIO_PIN_1);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_1,GPIO_PIN_RESET);
  
    /* USER CODE END 2 */  
  
    /* Infinite loop */  
    /* USER CODE BEGIN WHILE */  
    while (1) {  
        /* USER CODE END WHILE */  
  
        /* USER CODE BEGIN 3 */ 
        drive(RC_Ctl.rc.channel0, RC_Ctl.rc.channel1, RC_Ctl.rc.channel2,RC_Ctl.rc.channel3,(buttonPos)RC_Ctl.rc.s1,(buttonPos)RC_Ctl.rc.s2);
        HAL_Delay(1); 
    }  
    /* USER CODE END 3 */  
}  
  
/** System Clock Configuration  
*/  
void SystemClock_Config(void)  
{  
  
    RCC_OscInitTypeDef RCC_OscInitStruct;  
    RCC_ClkInitTypeDef RCC_ClkInitStruct;  
  
    /**Configure the main internal regulator output voltage   
    */  
    __HAL_RCC_PWR_CLK_ENABLE();  
  
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  
  
    /**Initializes the CPU, AHB and APB busses clocks   
    */  
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;  
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;  
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;  
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;  
    RCC_OscInitStruct.PLL.PLLM = 6;  
    RCC_OscInitStruct.PLL.PLLN = 168;  
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  
    RCC_OscInitStruct.PLL.PLLQ = 4;  
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {  
        _Error_Handler(__FILE__, __LINE__);  
    }  
  
    /**Initializes the CPU, AHB and APB busses clocks   
    */  
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK  
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;  
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;  
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;  
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {  
        _Error_Handler(__FILE__, __LINE__);  
    }  
  
    /**Configure the Systick interrupt time   
    */  
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);  
  
    /**Configure the Systick   
    */  
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);  
  
    /* SysTick_IRQn interrupt configuration */  
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);  
}  
  
/* USER CODE BEGIN 4 */  
  
  
void drive_pnuemb(buttonPos b,GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	switch(b){
		case UP: HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);break;
		case DOWN:
		case MID: HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);break;	
	}
}

void drive_pnuemb2(buttonPos b,GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	switch(b){
		case UP: HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);break;
		case DOWN:
		case MID: HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);break;	
	}
}

void drive_pnuemc(int16_t channel,GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	if(channel<0)
		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);
	else
		
		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);
}

 void drive(int RX_X2, int RX_Y1, int RX_X1,int RX_Y2,buttonPos b1, buttonPos b2){
	 
	 int16_t chan0  = (int16_t)map(RX_X2, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
	 int16_t chan1  = (int16_t)map(RX_Y1, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
	 int16_t chan2  = (int16_t)map(RX_X1, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
	 int16_t chan3  = (int16_t)map(RX_Y2, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
	switch(b1){
		case UP: drive_kinematics(RX_X2, RX_Y1, RX_X1,(int)b2);break;
		case DOWN: drive_kinematics(RX_X2, RX_Y1, 1024,3);
			drive_pnuemb(b2,GPIOA,GPIO_PIN_5); 
							 drive_pnuemc(chan3,GPIOE,GPIO_PIN_6);
		break;
		case MID: drive_kinematics(RX_X2, RX_Y1, 1024,3);
			drive_pnuemb2(b2,GPIOE,GPIO_PIN_5);
							drive_pnuemc(chan3,GPIOF,GPIO_PIN_1);
		break;
	}
}

void GPIO_PP_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_Pin;


	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx,&GPIO_InitStruct);
}

  
/* USER CODE END 4 */  
  
/**  
  * @brief  Period elapsed callback in non blocking mode  
  * @note   This function is called  when TIM6 interrupt took place, inside  
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment  
  * a global variable "uwTick" used as application time base.  
  * @param  htim : TIM handle  
  * @retval None  
  */  
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  
{  
/* USER CODE BEGIN Callback 0 */  
  
/* USER CODE END Callback 0 */  
    if (htim->Instance == TIM6) {  
        HAL_IncTick();  
    }  
/* USER CODE BEGIN Callback 1 */  
  
    //HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, GPIO_PIN_RESET);  
/* USER CODE END Callback 1 */  
}  
  
/**  
  * @brief  This function is executed in case of error occurrence.  
  * @param  None  
  * @retval None  
  */  
void _Error_Handler(char *file, int line)  
{  
    /* USER CODE BEGIN Error_Handler_Debug */  
    /* User can add his own implementation to report the HAL error return state */  
    while (1) {  
    }  
    /* USER CODE END Error_Handler_Debug */  
}  
  
#ifdef USE_FULL_ASSERT  
  
/**  
   * @brief Reports the name of the source file and the source line number  
   * where the assert_param error has occurred.  
   * @param file: pointer to the source file name  
   * @param line: assert_param error line source number  
   * @retval None  
   */  
void assert_failed(uint8_t* file, uint32_t line)  
{  
  /* USER CODE BEGIN 6 */  
  /* User can add his own implementation to report the file name and line number,  
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */  
  /* USER CODE END 6 */  
  
}  
  
#endif  
  
/**  
  * @}  
  */  
  
/**  
  * @}  
*/  
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/  

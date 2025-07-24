/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd2004.h"
#include "25LC512.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    EEPROM_HandleTypeDef E2PROM; 
    Lcd_HandleTypeDef hlcd1;
    RTC_TimeTypeDef theTime;
    RTC_DateTypeDef theDate;
    float Temp;     
    float Light;           
    float Moisture;    
    uint8_t DoorStatus;    
} SensorDataType;
typedef struct {
    uint8_t light;
    uint8_t moisture;
    uint8_t temp;
} CurrentDangerType;
typedef struct {
    uint8_t light;
    uint8_t moisture;
    uint8_t temp;
} PreviousDangerType;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define HAL_stuffs() HAL_TIM_Base_Start_IT(&htim1);                               \
                     HAL_ADC_Start_DMA(&hadc1, AChannels, sizeof(AChannels));     \
                     HAL_TIM_PWM_Start(&htim2, Heater_CH);                        \
                     HAL_TIM_PWM_Start(&htim2, Curtain_CH);                       \
                     HAL_RTC_GetTime(&hrtc, &SensorData.theTime, RTC_FORMAT_BIN); \
                     HAL_RTC_GetDate(&hrtc, &SensorData.theDate, RTC_FORMAT_BIN)

#define LCD_Init() Lcd_PortType D_ports[] = {portD4 , portD5, portD6, portD7};                                \
                   Lcd_PinType D_pins[] = {pinD4 , pinD5, pinD6, pinD7};                                      \
                   SensorData.hlcd1 = Lcd_create(D_ports, D_pins, portRS, pinRS, portE, pinE, LCD_4_BIT_MODE)

#define E2PROM_Init() EEPROM_Init(&SensorData.E2PROM, &hspi1, CS_GPIO_Port, CS_Pin)
                     
#define EEPROM_Write(writeAddress, data) EEPROM_Write(&SensorData.E2PROM, writeAddress, (uint8_t *)data, strlen(data));
                     
#define LCD(row, column, text) Lcd_cursor(&SensorData.hlcd1, row, column);\
                               Lcd_string(&SensorData.hlcd1, text)
      
#define ADCtoPercentage(Channel) (float)Channel * (100.0 / 4095.0)

#define ADCtoCentigrad(Channel) ((float)Channel * (5.0 / 4095.0) / 0.01) - tune_offset

#define FanControler(temp, m, M) (temp >= m)                                                        \
                                    ? HAL_GPIO_WritePin(Fan_GPIO_Port, Fan_Pin, GPIO_PIN_SET)       \
                                    : ((temp <= M)                                                  \
                                        ? HAL_GPIO_WritePin(Fan_GPIO_Port, Fan_Pin, GPIO_PIN_RESET) \
                                        : __NOP())                                                  \

#define  PumpControler(moisture, m, M) (moisture <= M)                                                      \
                                          ? HAL_GPIO_WritePin(Pump_GPIO_Port, Pump_Pin, GPIO_PIN_SET)       \
                                          : ((moisture >= m)                                                \
                                              ? HAL_GPIO_WritePin(Pump_GPIO_Port, Pump_Pin, GPIO_PIN_RESET) \
                                              : __NOP())                                                                        
                                          
#define PWMCurtain(light, m, M) pwm_value = (uint32_t)((light <= M)                     \
                                                      ? ((M - light) * 0.01 * 65535)    \
                                                      : ((light >= m)                   \
                                                          ? 0                           \
                                                          : (10.0 * 65535.0) * 0.01));  \
                                __HAL_TIM_SET_COMPARE(&htim2, Curtain_CH, pwm_value)
                                
#define PWMHeater(temp, m, M) pwm_value = (uint32_t)((temp < M)                              \
                                                      ? ((M - temp) * 10.0 * 0.01 * 65535.0) \
                                                      : ((temp >= m)                         \
                                                          ? 0                                \
                                                          : (5.0 * 65535.0) * 0.01));        \
                              __HAL_TIM_SET_COMPARE(&htim2, Heater_CH, pwm_value)                                

#define GSM_Init() HAL_UART_Transmit(&huart1, "> AT\r\n", strlen("> AT\r\n"), 1000);               \
                   uint8_t condition;                                                              \
                   do {                                                                            \
                        condition = HAL_UART_Receive(&huart1, OK, sizeof(OK), 1000);               \
                      } while (condition);                                                         \
                   HAL_UART_Transmit(&huart1, "OK\r\n", strlen("OK\r\n"), 1000);               \
                   HAL_UART_Transmit(&huart1, "> AT+CMGF=1\r\n" , strlen("> AT+CMGF=1\r\n" ), 1000)
                
#define GSM_Send_Message(text, number) sprintf((char *)Message, "> AT+CMGS=%s\r\n> %s\r\n> Ctrl+Z\r\n", number, text); \
                                       HAL_UART_Transmit(&huart1, Message, strlen((char const *)Message), 1000)                                                              
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
SensorDataType SensorData;

char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

int hours[] = {12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

char* meridiem[] = {"AM", "PM"};

char printer0 [21];
char printer1 [100];

uint32_t AChannels[3];

uint32_t pwm_value;

uint16_t last_adr = 0;

static PreviousDangerType previousDanger = {0, 0, 0};
static CurrentDangerType currentDanger = {0, 0, 0};

uint8_t OK[] = {"OK\r\n"};
uint8_t Message[] = {"> AT+CMGS=%s\r\n> %s\r\n> Ctrl+Z\r\n"};

uint32_t doorOpenTime = 0;
uint8_t doorWasOpen = 0;

enum {yes, no} write;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1)
  {
    HAL_RTC_GetTime(&hrtc, &SensorData.theTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &SensorData.theDate, RTC_FORMAT_BIN);
    
    sprintf(printer1, "{@(%2d:%2d)->Light(%%):%5.1f&Mois(%%):%5.1f&Temp('C):%5.1f&Door:%1d}  ",SensorData.theTime.Hours,
                                                                                           SensorData.theTime.Minutes,
                                                                                           SensorData.Light,
                                                                                           SensorData.Moisture,
                                                                                           SensorData.Temp,
                                                                                           SensorData.DoorStatus);
    
    write = yes;
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  HAL_stuffs();  
  LCD_Init();
  GSM_Init();     
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    E2PROM_Init();
        
    sprintf(printer0 ,"%d:%02d %s\0  %s,%s %d", hours[SensorData.theTime.Hours % 12],
    /* \0 is intentional to truncate for LCD */ SensorData.theTime.Minutes, 
                                                meridiem[SensorData.theTime.Hours / 12], 
                                                days[SensorData.theDate.WeekDay], 
                                                months[SensorData.theDate.Month-1], 
                                                SensorData.theDate.Date);
    LCD(0, 0, printer0);
    SensorData.DoorStatus = (HAL_GPIO_ReadPin(Door_Port, Door_Pin) == GPIO_PIN_SET);
    LCD(0, 15, SensorData.DoorStatus ? " Open": "Close");
    if (SensorData.DoorStatus && !doorWasOpen)
    {
        doorOpenTime = SensorData.theTime.Seconds;
        doorWasOpen = 1;
    }
    if (SensorData.DoorStatus)
    {
        uint32_t elapsed = (SensorData.theTime.Seconds >= doorOpenTime) ?
                           (SensorData.theTime.Seconds - doorOpenTime) :
                           (60 - doorOpenTime +SensorData.theTime.Seconds);

        if (elapsed > 5)
        {
            GSM_Send_Message("Warning: The door is open for more than 5 seconds!", "+9812345678912");
            doorOpenTime = SensorData.theTime.Seconds + 5;
        }
    }
    else
    {
        doorWasOpen = 0;
    }
    
    SensorData.Light = ADCtoPercentage(AChannels[0]);
    sprintf(printer0, "Light(%%): %-4.1f", SensorData.Light);
    LCD(1, 0, printer0);
    currentDanger.light = (SensorData.Light <= 5.0 || SensorData.Light >= 60.0);
    if (currentDanger.light != previousDanger.light)
    {
      if (currentDanger.light)
      {
        GSM_Send_Message("Danger zone for Light!", "+9812345678912");
      }
      previousDanger.light = currentDanger.light;
    }
        
    SensorData.Moisture = ADCtoPercentage(AChannels[1]);   
    sprintf(printer0, "Moisture(%%): %-4.1f", SensorData.Moisture);
    LCD(2, 0, printer0);
    currentDanger.moisture = (SensorData.Moisture <= 15.0 || SensorData.Moisture >= 90.0);
    if (currentDanger.moisture != previousDanger.moisture)
    {
      if (currentDanger.moisture)
      {
        GSM_Send_Message("Danger zone for Moisture!", "+9812345678912");
      }
      previousDanger.moisture = currentDanger.moisture;
    }  
    
    SensorData.Temp = ADCtoCentigrad(AChannels[2]);
    sprintf(printer0, "Temp('C): %-4.1f", SensorData.Temp);
    LCD(3, 0, printer0);
    currentDanger.temp = (SensorData.Temp <= 15.0 || SensorData.Temp  >= 40.0);
    if (currentDanger.temp != previousDanger.temp)
    {
      if (currentDanger.temp)
      {
        GSM_Send_Message("Danger zone for Temp!", "+9812345678912");
      }
      previousDanger.temp = currentDanger.temp;
    } 
    
    FanControler(SensorData.Temp, FanOn, FanOff);
    
    PumpControler(SensorData.Moisture, PumpOff, PumpOn);
    
    PWMCurtain(SensorData.Light, 50.0, 10.0);
   
    PWMHeater(SensorData.Temp, 25.0, 20.0);
    
    if (write == yes) 
    {  
      write = no;
      EEPROM_Write(E2PROM_writeAddress+last_adr, printer1);
      last_adr += strlen(printer1);
    }
        
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

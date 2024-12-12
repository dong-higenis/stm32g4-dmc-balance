/*
 * hw_def.h
 *
 *  Created on: Dec 4, 2024
 *      Author: user
 */

#ifndef AP_HW_DEF_H_
#define AP_HW_DEF_H_


#include "main.h"
#include "def.h"


#define _DEF_FIRMWATRE_VERSION    "V1G431R1"
#define _DEF_BOARD_NAME           "STM32G4-DMC-BALANCE"


// 하드웨어 정의 추가
#define _USE_HW_ICM42670
#define _USE_HW_IMU

#define _USE_HW_LED
#define      HW_LED_MAX_CH        1

#define _USE_HW_UART
#if 0
#define      HW_UART_MAX_CH         5
#define      HW_UART_CH_LPUART      _DEF_UART1
#define      HW_UART_CH_LIN         _DEF_UART2
#define      HW_UART_CH_RS232       _DEF_UART3
#define      HW_UART_CH_RS485       _DEF_UART4
#define      HW_UART_CH_USB         _DEF_UART5
#else
#define      HW_UART_MAX_CH         2
#define      HW_UART_CH_DEBUG       _DEF_UART1
#define      HW_UART_CH_2           _DEF_UART2
#endif

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    32
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    8
#define      HW_CLI_LINE_BUF_MAX    64

//#define _USE_HW_CLI_GUI
//#define      HW_CLI_GUI_WIDTH       80
//#define      HW_CLI_GUI_HEIGHT      24

#define _USE_HW_LOG
#define      HW_LOG_CH              HW_UART_CH_DEBUG
#define      HW_LOG_BOOT_BUF_MAX    2048
#define      HW_LOG_LIST_BUF_MAX    4096

#define _USE_HW_GPIO
#define      HW_GPIO_CH_BTN         0
#define      HW_GPIO_MAX_CH         10

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       1

#define _USE_HW_SWTIMER
#define      HW_SWTIMER_MAX_CH      8

#define _USE_HW_CAN
#define      HW_CAN_MAX_CH          1
#define      HW_CAN_MSG_RX_BUF_MAX  32

#define _USE_HW_PWM
#define      HW_PWM_MAX_CH          2

#define _USE_HW_MOTOR
#define      HW_MOTOR_MAX_CH        2

#define _USE_HW_ENCODER
#define      HW_ENCODER_MAX_CH      2

#define _USE_HW_I2C
#define      HW_I2C_MAX_CH          1
#define      HW_I2C_CH_IMU          _DEF_I2C1

#define _USE_HW_SERVO
#define      HW_SERVO_MAX_CH        2

#define _USE_HW_PS2


//-- CLI
#define _USE_CLI_HW_PWM             1
#define _USE_CLI_HW_MOTOR           1
#define _USE_CLI_HW_ENCODER         1
#define _USE_CLI_HW_SERVO           1
#define _USE_CLI_HW_PS2             1

typedef enum
{
  BTN,
  MT_R_IN_1,
  MT_R_IN_2,
  MT_L_IN_1,
  MT_L_IN_2,
  R_ENC1,
  R_ENC2,
  L_ENC1,
  L_ENC2,
  SPI_CS,
  GPIO_PIN_MAX
} GpioPinName_t;


typedef enum
{
  mPWM_MT_L_IN,
  mPWM_MT_R_IN,
  mPWM_MAX
} PwmPinName_t;



// 공통 사용 함수
//#define logPrintf printf

void     delay(uint32_t ms);
uint32_t millis(void);
uint32_t micros(void);

#endif /* AP_HW_DEF_H_ */

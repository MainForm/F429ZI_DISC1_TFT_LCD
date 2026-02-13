
#include "adc.h"
#include "cmsis_os.h"

#include "fmc.h"
#include "font/fonts.hpp"
#include "main.h"

#include "gpio.h"
#include "spi.h"
#include "ltdc.h"
#include "dma2d.h"
#include "stm32f4xx_hal_def.h"
#include "usart.h"

#include "ILI9341.hpp"
#include "IS42S16400J_7TL.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_uart.h"

#include <cstdint>
#include <cstdio>
#include <stdint.h>
#include <string.h>


extern osThreadId defaultTaskHandle;

TFT_LCD::ILI9341 lcd(
    {
        .hspi = &hspi5,
        .CS = {CSX_GPIO_Port,CSX_Pin},   // CS
        .WR = {WRX_DCX_GPIO_Port,WRX_DCX_Pin},   // WR
        .RD = {RDX_GPIO_Port,RDX_Pin},    // RD
        .hltdc = &hltdc
    }
);


#define FB_ADDR         ((uint32_t)0xD0000000)
#define FB_BACK_ADDR    ((uint32_t)FB_ADDR + (TFT_LCD::ILI9341::LCD_HEIGHT * TFT_LCD::ILI9341::LCD_WIDTH * TFT_LCD::ILI9341::PIXEL_BYTE_COUNT))

static void LCD_Fill_DMA2D()
{
    if (HAL_DMA2D_Start(
            &hdma2d,
            (uint32_t)FB_BACK_ADDR,   // Source
            (uint32_t)FB_ADDR,  // Destination (LTDC가 읽음)
            TFT_LCD::ILI9341::LCD_WIDTH,
            TFT_LCD::ILI9341::LCD_HEIGHT) != HAL_OK) {
        Error_Handler();
    }

    HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
}

uint16_t joystickPosition[2];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(defaultTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

extern "C"
void StartDefaultTask(void const * argument){

    // SDRAM 초기화
    BSP_SDRAM_IS42S16400J_7TL_Init(&hsdram1, REFRESH_COUNT);

    // LCD 초기화
    lcd.initalize(reinterpret_cast<uint16_t*>(FB_ADDR));

    // 더블 버퍼링을 위한 Backbuffer 설정
    lcd.setBackFrameBuffer(reinterpret_cast<uint16_t*>(FB_BACK_ADDR));

    char msg[64] = "";

    for(;;){
        // 조이스틱 좌표 입력
        HAL_ADC_Start_DMA(&hadc1,(uint32_t*)joystickPosition,2);
        
        // backframe에 그리기
        lcd.drawRectangle(10, 10, 240, 60, 0x0000,false);
        sprintf(msg, "X : %d",joystickPosition[0]);
        lcd.putText(msg,10, 10, Font20, 0xFFFF,false);
        sprintf(msg, "y : %d",joystickPosition[1]);
        lcd.putText(msg,10, 10 + Font20.Height, Font20, 0xFFFF,false);

        // 프레임 업데이트
        lcd.updateFrame();

        sprintf(msg, "%d %d\r\n",joystickPosition[0],joystickPosition[1]);
        HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
        osDelay(33);
    }
}
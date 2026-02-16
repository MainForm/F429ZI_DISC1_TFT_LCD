#include "main.h"

// peripheral headers
#include "adc.h"
#include "fmc.h"
#include "gpio.h"
#include "spi.h"
#include "ltdc.h"
#include "dma2d.h"
#include "usart.h"
#include "i2c.h"

// TFT_LCD Headers
#include "ILI9341.hpp"
#include "font/fonts.hpp"

// TouchPanel Header
#include "STMPE811.hpp"

// External SDRAM Header
#include "IS42S16400J_7TL.h"

// FreeRTOS Header
#include "cmsis_os.h"

// Standard Headers
#include <cstdint>
#include <cstdio>
#include <stdint.h>
#include <string.h>

#define FB_ADDR         ((uint32_t)0xD0000000)
#define FB_BACK_ADDR    ((uint32_t)FB_ADDR + (TFT_LCD::ILI9341::LCD_HEIGHT * TFT_LCD::ILI9341::LCD_WIDTH * TFT_LCD::ILI9341::PIXEL_BYTE_COUNT))

TFT_LCD::ILI9341 lcd(
    TFT_LCD::ILI9341_Config{
        .hspi = &hspi5,
        .CS = {CSX_GPIO_Port,CSX_Pin},              // CS
        .WR = {WRX_DCX_GPIO_Port,WRX_DCX_Pin},      // WR
        .RD = {RDX_GPIO_Port,RDX_Pin},              // RD
        .hltdc = &hltdc,
        .hdma2d = &hdma2d
    }
);

TFT_LCD::STMPE811 touchPanel(&hi2c3);

uint32_t joystickPosition[2] {5,5};

#define TP_WIDTH    (3800)
#define TP_HEIGHT   (4000)

extern "C"
void StartDefaultTask(void const * argument){

    // SDRAM 초기화
    BSP_SDRAM_IS42S16400J_7TL_Init(&hsdram1, REFRESH_COUNT);

    // LCD 초기화
    lcd.initalize(reinterpret_cast<uint16_t*>(FB_ADDR));

    // 더블 버퍼링을 위한 Backbuffer 설정
    lcd.setBackFrameBuffer(reinterpret_cast<uint16_t*>(FB_BACK_ADDR));

    touchPanel.initalize();

    char msg[64] = "";

    for(;;){
        // 조이스틱 좌표 입력
        // HAL_ADC_Start_DMA(&hadc1,(uint32_t*)joystickPosition,2);

        if(touchPanel.isTouched() == true){
            touchPanel.getTouchedPoint(joystickPosition[0], joystickPosition[1]);

            joystickPosition[0] *= (TFT_LCD::ILI9341::LCD_WIDTH / (float)TP_WIDTH);
            joystickPosition[1] *= (TFT_LCD::ILI9341::LCD_HEIGHT / (float)TP_HEIGHT);

            joystickPosition[0] = TFT_LCD::ILI9341::LCD_WIDTH - joystickPosition[0];
            joystickPosition[1] = TFT_LCD::ILI9341::LCD_HEIGHT - joystickPosition[1];
        }
        
        // backframe에 그리기
        lcd.drawRectangle(joystickPosition[0] - 5, joystickPosition[1] -5 , 10, 10, 0xFFFF);

        // 프레임 업데이트
        lcd.updateFrame();

        sprintf(msg, "%d %d\r\n",joystickPosition[0],joystickPosition[1]);
        HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
        osDelay(33);
    }
}
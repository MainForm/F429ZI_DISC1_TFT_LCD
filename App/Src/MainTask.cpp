
#include "cmsis_os.h"

#include "fmc.h"
#include "font/fonts.hpp"
#include "main.h"

#include "gpio.h"
#include "spi.h"
#include "ltdc.h"
#include "dma2d.h"

#include "ILI9341.hpp"
#include "IS42S16400J_7TL.h"

#include <cstdint>
#include <stdint.h>
#include <string.h>

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
#define FB_BACK_ADDR    ((uint32_t)0xD004B000)

// static void LCD_Fill_DMA2D(uint32_t buffer, uint32_t back)
// {
//     if (HAL_DMA2D_Start(
//             &hdma2d,
//             (uint32_t)back,   // Source
//             (uint32_t)buffer,  // Destination (LTDC가 읽음)
//             LCD_WIDTH,
//             LCD_HEIGHT) != HAL_OK) {
//         Error_Handler();
//     }

//     HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
// }

extern "C"
void StartDefaultTask(void const * argument){

    // SDRAM 초기화
    BSP_SDRAM_IS42S16400J_7TL_Init(&hsdram1, REFRESH_COUNT);

    // LCD 초기화
    lcd.initalize(reinterpret_cast<uint16_t*>(FB_ADDR));

    // 문자열 출력
    lcd.putText("Hello World", 10, 10, Font12, 0xFFFF);
    lcd.putText("TFT_LCD Test", 10, Font20.Height + 10, Font12, 0xFFFF);  
    
    // 파랑색 사각형
    lcd.drawRectangle(10, 50, 100, 100, TFT_LCD::Pixel(0x1F, 0x00, 0x00));

    for(;;){
        osDelay(10);
    }
}
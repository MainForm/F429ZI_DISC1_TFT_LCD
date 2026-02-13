
#include "ILI9341.hpp"

#include <array>
#include <cstdint>
#include <cstring>

#ifdef USE_FREERTOS
#include "cmsis_os.h"
#endif

#ifdef USE_HAL
[[noreturn]]
static void FatalError(const char* file, int line)
{
    __disable_irq();
    while (1) {
        // optional: LED blink, breakpoint
    }
}

#define ASSERT_PARAM(x) \
    do { if ((x) == nullptr) FatalError(__FILE__, __LINE__); } while(0)

#define DELAY_LCD(ms) HAL_Delay(ms)

#endif

#ifdef USE_FREERTOS

#include "cmsis_os.h"

#define ASSERT_PARAM(x)     configASSERT(x)
#define DELAY_LCD(ms)       osDelay(ms)

#endif

namespace TFT_LCD {
    // private method
    void ILI9341::writeCommand(const uint8_t command){
        HAL_GPIO_WritePin(config.WR.Port, config.WR.Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(config.CS.Port, config.CS.Pin, GPIO_PIN_RESET);

        HAL_SPI_Transmit(config.hspi, &command, 1, 10);
        HAL_GPIO_WritePin(config.CS.Port, config.CS.Pin, GPIO_PIN_SET);
    }

    void ILI9341::writeData(const uint8_t data){
        HAL_GPIO_WritePin(config.WR.Port, config.WR.Pin, GPIO_PIN_SET);

        HAL_GPIO_WritePin(config.CS.Port, config.CS.Pin, GPIO_PIN_RESET);

        HAL_SPI_Transmit(config.hspi, &data, 1, 10);

        HAL_GPIO_WritePin(config.CS.Port, config.CS.Pin, GPIO_PIN_SET);
    }

    void ILI9341::writeRegister(const uint8_t address,std::span<const uint8_t> data){
        writeCommand(address);

        for(const uint8_t& param : data){
            writeData(param);
        }
    }

    // public method
    ILI9341::ILI9341(ILI9341_Config config)
        : config{config}
    {
        ASSERT_PARAM(config.hspi != nullptr);
        ASSERT_PARAM(config.CS.Port != nullptr);
        ASSERT_PARAM(config.WR.Port != nullptr);
        ASSERT_PARAM(config.hltdc != nullptr);
    }

    /*
    The ILI9341 LCD initialization process is based on the STM32 ILI9341 library.
    URL : https://github.com/STMicroelectronics/stm32-ili9341/blob/main/ili9341.c
    */
    void ILI9341::initalize(uint16_t* FrameBufferAddress){

        /* Configure LCD */
        {
            writeRegister(0xCA, 
                std::to_array<uint8_t>(
                    {
                        0xC3, 
                        0x08, 
                        0x50
                    }
                )
            );

            writeRegister(POWERB,
                std::to_array<uint8_t>(
                    {
                        0x00, 
                        0xC1, 
                        0x30
                    }
                )
            );

            writeRegister(POWER_SEQ,
                std::to_array<uint8_t>(
                    {
                        0x64, 
                        0x03, 
                        0x12, 
                        0x81
                    }
                )
            );

            writeRegister(DTCA,
                std::to_array<uint8_t>(
                    {
                        0x85, 
                        0x00, 
                        0x78
                    }
                )
            );

            writeRegister(POWERA,
                std::to_array<uint8_t>(
                    {
                        0x39, 
                        0x2C, 
                        0x00, 
                        0x34,
                        0x02
                    }
                )
            );

            writeRegister(PRC,
                std::to_array<uint8_t>(
                    {
                        0x20
                    }
                )
            );

            writeRegister(DTCB,
                std::to_array<uint8_t>(
                    {
                        0x00,
                        0x00
                    }
                )
            );

            // Frame Rate Control : default 70Hz
            writeRegister(FRMCTR1,
                std::to_array<uint8_t>(
                    {
                        0x00,
                        0x1B
                    }
                )
            );

            writeRegister(DFC,
                std::to_array<uint8_t>(
                    {
                        0x0A,
                        0xA2
                    }
                )
            );

            writeRegister(POWER1,
                std::to_array<uint8_t>(
                    {
                        0x10
                    }
                )
            );

            writeRegister(POWER2,
                std::to_array<uint8_t>(
                    {
                        0x10
                    }
                )
            );

            writeRegister(VCOM1,
                std::to_array<uint8_t>(
                    {
                        0x45,
                        0x15
                    }
                )
            );

            writeRegister(VCOM2,
                std::to_array<uint8_t>(
                    {
                        0x90
                    }
                )
            );

            writeRegister(MAC,
                std::to_array<uint8_t>(
                    {
                        0xC8
                    }
                )
            );

            writeRegister(GAMMA3_EN,
                std::to_array<uint8_t>(
                    {
                        0x00
                    }
                )
            );

            writeRegister(RGB_INTERFACE,
                std::to_array<uint8_t>(
                    {
                        0xC2
                    }
                )
            );

            writeRegister(DFC,
                std::to_array<uint8_t>(
                    {
                        0x0A,
                        0xA7,
                        0x27,
                        0x04
                    }
                )
            );


            writeRegister(COLUMN_ADDR,
                std::to_array<uint8_t>(
                    {
                        0x00,
                        0x00,
                        0x00,
                        0xEF
                    }
                )
            );

            writeRegister(PAGE_ADDR,
                std::to_array<uint8_t>(
                    {
                        0x00,
                        0x00,
                        0x01,
                        0x3F
                    }
                )
            );

            writeRegister(INTERFACE,
                std::to_array<uint8_t>(
                    {
                        0x01,
                        0x00,
                        0x06
                    }
                )
            );


            writeCommand(GRAM);
            DELAY_LCD(200);
    
            writeRegister(GAMMA,
                std::to_array<uint8_t>(
                    {
                        0x01
                    }
                )
            );
    
            writeRegister(PGAMMA,
                std::to_array<uint8_t>(
                    {
                        0x0F,
                        0x29,
                        0x24,
                        0x0C,
                        0x0E,
                        0x09,
                        0x4E,
                        0x78,
                        0x3C,
                        0x09,
                        0x13,
                        0x05,
                        0x17,
                        0x11,
                        0x00
                    }
                )
            );

            writeRegister(NGAMMA,
                std::to_array<uint8_t>(
                    {
                        0x00,
                        0x16,
                        0x1B,
                        0x04,
                        0x11,
                        0x07,
                        0x31,
                        0x33,
                        0x42,
                        0x05,
                        0x0C,
                        0x0A,
                        0x28,
                        0x2F,
                        0x0F
                    }
                )
            );

            writeCommand(SLEEP_OUT);
            DELAY_LCD(200);

            writeCommand(DISPLAY_ON);
            writeCommand(GRAM);
        }

        FrameBuffer frameBuffer(FrameBufferAddress,LCD_WIDTH,LCD_HEIGHT);
        _FrameBuffer[0] = frameBuffer;

        setLayer(0,frameBuffer);
    }

    void ILI9341::setBackFrameBuffer(uint16_t* FrameBufferAddress){
        FrameBuffer backFrame(FrameBufferAddress,LCD_WIDTH,LCD_HEIGHT);

        _FrameBuffer[1] = backFrame;

        _hasBackFrame = true;
    }

    void ILI9341::setLayer(uint32_t layerIndex,const FrameBuffer& frameBuffer, uint32_t left, uint32_t top){
        LTDC_LayerCfgTypeDef Layercfg;

        /* Layer Init */
        Layercfg.WindowX0 = left;
        Layercfg.WindowX1 = left + frameBuffer.getWidth();
        Layercfg.WindowY0 = top;
        Layercfg.WindowY1 = top + frameBuffer.getHeight();
        Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
        Layercfg.FBStartAdress = reinterpret_cast<uint32_t>(frameBuffer.getBufferAddress());
        Layercfg.Alpha = 255;

        Layercfg.Alpha0 = 0;
        Layercfg.Backcolor.Blue = 0;
        Layercfg.Backcolor.Green = 0;
        Layercfg.Backcolor.Red = 0;
        Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
        Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
        Layercfg.ImageWidth = frameBuffer.getWidth();
        Layercfg.ImageHeight = frameBuffer.getHeight();

        if(HAL_LTDC_ConfigLayer(config.hltdc, &Layercfg, layerIndex) != HAL_OK){
            ASSERT_PARAM(false);
        }

        /* Dithering activation */
        HAL_LTDC_EnableDither(config.hltdc);
    }

    void ILI9341::drawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Pixel color,bool update){
        uint32_t curFrameBufferIdx = _selectedFrameBuffer;

        if(_hasBackFrame == true){
            curFrameBufferIdx = !curFrameBufferIdx;
        }

        _FrameBuffer[curFrameBufferIdx].drawRectangle(x, y, width, height, color);
        _isUpdatedRecently = false;

        if(_hasBackFrame == true && update == true){
            updateFrame();
        }
    }

    void ILI9341::putText(std::string text, uint32_t x,uint32_t y,const sFONT& font,Pixel color,bool update){
        uint32_t curFrameBufferIdx = _selectedFrameBuffer;

        if(_hasBackFrame == true){
            curFrameBufferIdx = !curFrameBufferIdx;
        }

        _FrameBuffer[curFrameBufferIdx].putText(text, x, y, font, color);
        _isUpdatedRecently = false;

        if(_hasBackFrame == true && update == true){
            updateFrame();
        }
    }

    void ILI9341::putChar(uint8_t character,uint32_t x,uint32_t y,const sFONT& font,Pixel color,bool update){
        uint32_t curFrameBufferIdx = _selectedFrameBuffer;

        if(_hasBackFrame == true){
            curFrameBufferIdx = !curFrameBufferIdx;
        }

        _FrameBuffer[curFrameBufferIdx].putChar(character, x, y, font, color);
        _isUpdatedRecently = false;
        
        if(_hasBackFrame == true && update == true){
            updateFrame();
        }
    }

    bool ILI9341::updateFrame(){
        if(_hasBackFrame == false){
            return false;
        }

        if(_isUpdatedRecently == true){
            return false;
        }
        
        _selectedFrameBuffer = !_selectedFrameBuffer;

        setLayer(0, _FrameBuffer[_selectedFrameBuffer]);

        _FrameBuffer[!_selectedFrameBuffer].copyBuffer(_FrameBuffer[_selectedFrameBuffer],config.hdma2d);
        _isUpdatedRecently = true;

        return true;
    }
}


#ifdef __cplusplus

#ifndef __ILI9341_LIB_H__
#define __ILI9341_LIB_H__

/*
This library is for ILI9341 TFT LCD Display builtin STM32F429I-DISC1.
If you use this library for other board.
You need to check schematic.

STM32F429I-DISC1 can not read data from builtin ILI9341.
Because MISO pin is not connected to ILI9341.

*/

#include "main.h"

#include "FrameBuffer.hpp"

#include <array>
#include <span>

// #define USE_HAL
#define USE_FREERTOS

namespace TFT_LCD {
    struct ILI9341_Config {
        SPI_HandleTypeDef* const hspi = nullptr;

        struct {
            GPIO_TypeDef* const Port = nullptr;
            uint16_t Pin = 0;
        } CS,WR,RD;

        LTDC_HandleTypeDef* const hltdc = nullptr;
    };

    class ILI9341{
    public:
        static const uint32_t LCD_WIDTH {240};
        static const uint32_t LCD_HEIGHT {320};
        static const uint32_t PIXEL_BYTE_COUNT {2};
    private:
        /** 
        * @brief  ILI9341 Registers  
        */
        enum Register : uint8_t {
            /* Level 1 Commands */
            SWRESET              = 0x01,   /* Software Reset */
            READ_DISPLAY_ID      = 0x04,   /* Read display identification information */
            RDDST                = 0x09,   /* Read Display Status */
            RDDPM                = 0x0A,   /* Read Display Power Mode */
            RDDMADCTL            = 0x0B,   /* Read Display MADCTL */
            RDDCOLMOD            = 0x0C,   /* Read Display Pixel Format */
            RDDIM                = 0x0D,   /* Read Display Image Format */
            RDDSM                = 0x0E,   /* Read Display Signal Mode */
            RDDSDR               = 0x0F,   /* Read Display Self-Diagnostic Result */
            SPLIN                = 0x10,   /* Enter Sleep Mode */
            SLEEP_OUT            = 0x11,   /* Sleep out register */
            PTLON                = 0x12,   /* Partial Mode ON */
            NORMAL_MODE_ON       = 0x13,   /* Normal Display Mode ON */
            DINVOFF              = 0x20,   /* Display Inversion OFF */
            DINVON               = 0x21,   /* Display Inversion ON */
            GAMMA                = 0x26,   /* Gamma register */
            DISPLAY_OFF          = 0x28,   /* Display off register */
            DISPLAY_ON           = 0x29,   /* Display on register */
            COLUMN_ADDR          = 0x2A,   /* Colomn address register */ 
            PAGE_ADDR            = 0x2B,   /* Page address register */ 
            GRAM                 = 0x2C,   /* GRAM register */   
            RGBSET               = 0x2D,   /* Color SET */   
            RAMRD                = 0x2E,   /* Memory Read */   
            PLTAR                = 0x30,   /* Partial Area */   
            VSCRDEF              = 0x33,   /* Vertical Scrolling Definition */   
            TEOFF                = 0x34,   /* Tearing Effect Line OFF */   
            TEON                 = 0x35,   /* Tearing Effect Line ON */   
            MAC                  = 0x36,   /* Memory Access Control register*/
            VSCRSADD             = 0x37,   /* Vertical Scrolling Start Address */   
            IDMOFF               = 0x38,   /* Idle Mode OFF */   
            IDMON                = 0x39,   /* Idle Mode ON */   
            PIXEL_FORMAT         = 0x3A,   /* Pixel Format register */
            WRITE_MEM_CONTINUE   = 0x3C,   /* Write Memory Continue */   
            READ_MEM_CONTINUE    = 0x3E,   /* Read Memory Continue */   
            SET_TEAR_SCANLINE    = 0x44,   /* Set Tear Scanline */   
            GET_SCANLINE         = 0x45,   /* Get Scanline */   
            WDB                  = 0x51,   /* Write Brightness Display register */
            RDDISBV              = 0x52,   /* Read Display Brightness */   
            WCD                  = 0x53,   /* Write Control Display register*/
            RDCTRLD              = 0x54,   /* Read CTRL Display */   
            WRCABC               = 0x55,   /* Write Content Adaptive Brightness Control */   
            RDCABC               = 0x56,   /* Read Content Adaptive Brightness Control */   
            WRITE_CABC           = 0x5E,   /* Write CABC Minimum Brightness */   
            READ_CABC            = 0x5F,   /* Read CABC Minimum Brightness */   
            READ_ID1             = 0xDA,   /* Read ID1 */
            READ_ID2             = 0xDB,   /* Read ID2 */
            READ_ID3             = 0xDC,   /* Read ID3 */

            /* Level 2 Commands */
            RGB_INTERFACE       = 0xB0,   /* RGB Interface Signal Control */
            FRMCTR1             = 0xB1,   /* Frame Rate Control (In Normal Mode) */
            FRMCTR2             = 0xB2,   /* Frame Rate Control (In Idle Mode) */
            FRMCTR3             = 0xB3,   /* Frame Rate Control (In Partial Mode) */
            INVTR               = 0xB4,   /* Display Inversion Control */
            BPC                 = 0xB5,   /* Blanking Porch Control register */
            DFC                 = 0xB6,   /* Display Function Control register */
            ETMOD               = 0xB7,   /* Entry Mode Set */
            BACKLIGHT1          = 0xB8,   /* Backlight Control 1 */
            BACKLIGHT2          = 0xB9,   /* Backlight Control 2 */
            BACKLIGHT3          = 0xBA,   /* Backlight Control 3 */
            BACKLIGHT4          = 0xBB,   /* Backlight Control 4 */
            BACKLIGHT5          = 0xBC,   /* Backlight Control 5 */
            BACKLIGHT7          = 0xBE,   /* Backlight Control 7 */
            BACKLIGHT8          = 0xBF,   /* Backlight Control 8 */
            POWER1              = 0xC0,   /* Power Control 1 register */
            POWER2              = 0xC1,   /* Power Control 2 register */
            VCOM1               = 0xC5,   /* VCOM Control 1 register */
            VCOM2               = 0xC7,   /* VCOM Control 2 register */
            NVMWR               = 0xD0,   /* NV Memory Write */
            NVMPKEY             = 0xD1,   /* NV Memory Protection Key */
            RDNVM               = 0xD2,   /* NV Memory Status Read */
            READ_ID4            = 0xD3,   /* Read ID4 */
            PGAMMA              = 0xE0,   /* Positive Gamma Correction register */
            NGAMMA              = 0xE1,   /* Negative Gamma Correction register */
            DGAMCTRL1           = 0xE2,   /* Digital Gamma Control 1 */
            DGAMCTRL2           = 0xE3,   /* Digital Gamma Control 2 */
            INTERFACE           = 0xF6,   /* Interface control register */

            /* Extend register commands */
            POWERA          = 0xCB,   /* Power control A register */
            POWERB          = 0xCF,   /* Power control B register */
            DTCA            = 0xE8,   /* Driver timing control A */
            DTCB            = 0xEA,   /* Driver timing control B */
            POWER_SEQ       = 0xED,   /* Power on sequence register */
            GAMMA3_EN       = 0xF2,   /* 3 Gamma enable register */
            PRC             = 0xF7,   /* Pump ratio control register */
        };

        static const uint32_t FRAME_BUFFER_COUNT = 2;

        FrameBuffer _FrameBuffer[FRAME_BUFFER_COUNT];

        uint32_t _selectedFrameBuffer = -1;

        bool _hasBackFrame = false;
    private:
        const ILI9341_Config config;
    private:
        void writeCommand(const uint8_t command);
        void writeData(const uint8_t data);

        void writeRegister(const uint8_t address,std::span<const uint8_t> data);

        void setLayer(uint32_t layerIndex,const FrameBuffer& frameBuffer, uint32_t left = 0,uint32_t top = 0);
    public:
        ILI9341(ILI9341_Config config);

        void initalize(uint16_t* FrameBufferAddress);

        void setBackFrameBuffer(uint16_t* FrameBufferAddress);

        void drawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Pixel color,bool update = true);

        void putText(std::string text, uint32_t x,uint32_t y,const sFONT& font,Pixel color,bool update = true);
        void putChar(uint8_t character,uint32_t x,uint32_t y,const sFONT& font,Pixel color,bool update = true);

        bool updateFrame();
    };
}

#endif // __ILI9341_LIB_H__

#endif // __cplusplus
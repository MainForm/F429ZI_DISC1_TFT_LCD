


#ifdef __cplusplus

#ifndef __STMPE811_LIB_H__
#define __STMPE811_LIB_H__

#include "main.h"

#include <vector>

namespace TFT_LCD {
    class STMPE811{
    public:
        static constexpr uint8_t STMPE811_DEFAULT_ADDRESS = 0x82;
    private:
        enum Register : uint8_t{
            CHIP_ID = 0x00,
            SYS_CTRL1 = 0x03,
            SYS_CTRL2 = 0x04,
            INT_CTRL = 0x09,
            INT_EN = 0x0A,
            INT_STA = 0x0B,
            GPIO_DIR = 0x0C,
            GPIO_ED = 0x0D,
            GPIO_RE = 0x0E,
            GPIO_FE = 0x0F,
            GPIO_DATA = 0x10,
            GPIO_AF = 0x1C,
            ADC_CTRL1 = 0x20,
            ADC_CTRL2 = 0x21,
            TSC_CTRL = 0x40,
            TSC_CFG = 0x41,
            FIFO_TH = 0x4A,
            FIFO_STA = 0x4B,
            FIFO_SIZE = 0x4C,
            TSC_FRACT_XYZ = 0x56,
            TSC_I_DRIVE = 0x58,
            TSC_DATA_XYZ = 0x73,
            TSC_DATA_XYZ1 = 0x74,
            TSC_DATA_XYZ2 = 0x75,
            TSC_DATA_XYZ3 = 0x76,
            TSC_DATA_XYZ4 = 0x77,
            TSC_DATA_XYZ5 = 0x78,
            TSC_DATA_XYZ6 = 0x79,
            TSC_DATA_XYZ_NON_AUTO = 0xd7
        };

        enum ADC_Clock_Freq : uint8_t {
            ADC_FREQ_1_625MHz = 0x00,
            ADC_FREQ_3_25MHz = 0x01,
            ADC_FREQ_6_5MHz = 0x10,
        };

        static constexpr uint8_t SYS_CTRL2_ADC = 0x01;
        static constexpr uint8_t SYS_CTRL2_TSC = 0x02;
        static constexpr uint8_t SYS_CTRL2_GPIO = 0x04;
        static constexpr uint8_t SYS_CTRL2_TS = 0x08;

        static constexpr uint8_t GPIO_AF_XU = 0x10;
        static constexpr uint8_t GPIO_AF_YU = 0x20;
        static constexpr uint8_t GPIO_AF_XD = 0x40;
        static constexpr uint8_t GPIO_AF_YD = 0x80;

    private:
        I2C_HandleTypeDef* const hi2c;
        const uint16_t _address;

    private:
        HAL_StatusTypeDef readRegister(Register reg, uint32_t size, std::vector<uint8_t>& data);
        HAL_StatusTypeDef writeRegister(Register reg,const std::vector<uint8_t>& data);
        
    public:
        STMPE811(I2C_HandleTypeDef* hi2c,uint16_t address = STMPE811_DEFAULT_ADDRESS);
        
        bool initalize();

        bool isConnected();

        void reset();

        void resetFIFO();
        
        void enableMode(uint8_t mode);

        void enableAF(uint8_t gpioPin);

        void setADCFreqency(ADC_Clock_Freq freqency);

        bool isTouched();

        bool getTouchedPoint(uint32_t& x, uint32_t& y);
    };
}

#endif // __STMPE811_LIB_H__

#endif // __cplusplus

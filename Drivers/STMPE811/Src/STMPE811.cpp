
#include "STMPE811.hpp"
#include "stm32f4xx_hal.h"

#include "cmsis_os.h"

namespace TFT_LCD {
    HAL_StatusTypeDef STMPE811::readRegister(Register memoryAddress, uint32_t size, std::vector<uint8_t>& data){
        return HAL_I2C_Mem_Read(hi2c, _address, memoryAddress, I2C_MEMADD_SIZE_8BIT, data.data(), size, HAL_MAX_DELAY);
    }

    HAL_StatusTypeDef STMPE811::writeRegister(Register memoryAddress, const std::vector<uint8_t>& data){
        if (data.empty()) {
            return HAL_OK;
        }
        // HAL expects a non-const buffer, so copy into a temporary.
        std::vector<uint8_t> buffer(data.begin(), data.end());
        return HAL_I2C_Mem_Write(hi2c, _address, memoryAddress, I2C_MEMADD_SIZE_8BIT, buffer.data(), buffer.size(), HAL_MAX_DELAY);
    }


    STMPE811::STMPE811(I2C_HandleTypeDef* hi2c,uint16_t address)
        : hi2c(hi2c),_address(address)
    {
    }

    bool STMPE811::initalize()
    {
        if(!isConnected()){
            return false;
        }

        reset();

        osDelay(10);

        enableMode(SYS_CTRL2_GPIO);

        enableAF(GPIO_AF_XD | GPIO_AF_YD | GPIO_AF_XU | GPIO_AF_YU );

        enableMode(SYS_CTRL2_TS | SYS_CTRL2_ADC);

        //Sampling Frequency : 80Hz, 12Bit
        writeRegister(Register::ADC_CTRL1, {0x49});

        osDelay(2);

        setADCFreqency(ADC_Clock_Freq::ADC_FREQ_3_25MHz);

        /* Select 2 nF filter capacitor */
        /* Configuration: 
            - Touch average control    : 4 samples
            - Touch delay time         : 500 uS
            - Panel driver setting time: 500 uS 
        */
        writeRegister(Register::TSC_CFG, {0x9A});

        /* Configure the Touch FIFO threshold: single point reading */
        writeRegister(Register::FIFO_TH, {0x01});

        /* Clear the FIFO memory content. */
        writeRegister(Register::FIFO_STA, {0x01});


        /* Put the FIFO back into operation mode  */
        writeRegister(Register::FIFO_STA, {0x00});

        /* Set the range and accuracy pf the pressure measurement (Z) : 
            - Fractional part :7 
            - Whole part      :1 
        */
        writeRegister(Register::TSC_FRACT_XYZ, {0x01});


        /* Set the driving capability (limit) of the device for TSC pins: 50mA */
        writeRegister(Register::TSC_I_DRIVE, {0x01 });

        /* Touch screen control configuration (enable TSC):
            - No window tracking index
            - XYZ acquisition mode
        */
        writeRegister(Register::TSC_CTRL, {0x01});

        /*  Clear all the status pending bits if any */
        writeRegister(Register::INT_STA, {0xFF});
        
        osDelay(2);

        return true;
    }
    bool STMPE811::isConnected(){
        std::vector<uint8_t> data(2);
        
        HAL_StatusTypeDef status = readRegister(Register::CHIP_ID, 2, data);

        if(status != HAL_OK){
            return false;
        }

        return (data[0] != 0x11 && data[1] != 0x81);
    }

    void STMPE811::reset(){
        writeRegister(Register::SYS_CTRL1, {0x02});
        osDelay(10); // Wait for reset to complete

        writeRegister(Register::SYS_CTRL1, {0x00});
        osDelay(2);
    }

    void STMPE811::enableMode(uint8_t mode){
        std::vector<uint8_t> data;
        readRegister(Register::SYS_CTRL2, 1, data);
        uint8_t sysCtrl2 = data[0];

        sysCtrl2 &= ~mode; // Clear the bits corresponding to the mode to enable

        writeRegister(Register::SYS_CTRL2, {sysCtrl2});
    }

    void STMPE811::enableAF(uint8_t gpioAF){
        std::vector<uint8_t> data;
        readRegister(Register::GPIO_AF, 1, data);
        uint8_t gpioAFReg = data[0];

        gpioAFReg &= ~gpioAF; // Clear the bits corresponding to the alternate function

        writeRegister(Register::GPIO_AF, {gpioAFReg});
    }

    void STMPE811::setADCFreqency(ADC_Clock_Freq freqency){
        writeRegister(Register::ADC_CTRL2, {freqency});
    }

    bool STMPE811::isTouched(){
        uint8_t state;
        uint8_t ret = 0;
        
        std::vector<uint8_t> result(1);

        readRegister(Register::TSC_CTRL , 1, result);

        state = result[0];

        state &= 0x80;
        
        if(state > 0)
        {
            readRegister(Register::FIFO_SIZE, 1, result);

            if(result[0] > 0){
                ret = 1;
            }
        }
        else
        {
            resetFIFO();
        }
        return ret == 1;
    }

    bool STMPE811::getTouchedPoint(uint32_t& x, uint32_t& y){
        if(isTouched() == false){
            return false;
        }

        std::vector<uint8_t> rawData(4);

        readRegister(Register::TSC_DATA_XYZ_NON_AUTO, 4, rawData);

        uint32_t uldataXYZ;

        uldataXYZ = (rawData[0] << 24)|(rawData[1] << 16)|(rawData[2] << 8)|(rawData[3] << 0);  
        x = (uldataXYZ >> 20) & 0x00000FFF;     
        y = (uldataXYZ >>  8) & 0x00000FFF;     
        
        resetFIFO();

        return true;
    }

    void STMPE811::resetFIFO(){
            /* Reset FIFO */
            writeRegister(Register::FIFO_STA, {0x01});
            /* Enable the FIFO again */
            writeRegister(Register::FIFO_STA, {0x00});
    }
}

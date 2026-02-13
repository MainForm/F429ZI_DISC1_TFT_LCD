#include "FrameBuffer.hpp"
#include "font/fonts.hpp"
#include "main.h"
#include <cstdint>
#include <cstring>


namespace TFT_LCD {
    FrameBuffer::FrameBuffer(uint16_t* const buffer, uint32_t width, uint32_t height,PixelFormat format)
        : _buffer{reinterpret_cast<Pixel*>(buffer)}, _width{width}, _height{height}, _pixelFormat(format)
    {
        drawRectangle(0, 0, _width, _height, 0x0000);
    }

    FrameBuffer& FrameBuffer::operator=(const FrameBuffer& other){
        if(this == &other){
            return *this;
        }

        _buffer = other._buffer;
        _width = other._width;
        _height = other._height;
        _pixelFormat = other._pixelFormat;
        return *this;
    }

    void FrameBuffer::setBuffer(Pixel* const newBuffer){
        _buffer = newBuffer;
    }

    Pixel& FrameBuffer::at(uint32_t x, uint32_t y){
        return _buffer[y * _width + x];
    }

    uint32_t FrameBuffer::getWidth() const {
        return _width;
    }

    void FrameBuffer::setWidth(uint32_t width){
        _width = width;
    }

    uint32_t FrameBuffer::getHeight() const {
        return _height;
    }

    void FrameBuffer::copyBuffer(FrameBuffer& other,DMA2D_HandleTypeDef * hdma2d){

        if(hdma2d == nullptr){
            memcpy(getBufferAddress(), other.getBufferAddress(),
                    _height*_width*_pixelFormat);
        }
        else{
                HAL_DMA2D_Start(
                        hdma2d,
                        (uint32_t)other.getBufferAddress(),   // Source
                        (uint32_t)this->getBufferAddress(),  // Destination (LTDC가 읽음)
                        _width,
                        _height);

                HAL_DMA2D_PollForTransfer(hdma2d, HAL_MAX_DELAY);
        }
    }

    void FrameBuffer::setHeight(uint32_t height){
        _height = height;
    }

    void FrameBuffer::drawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Pixel color){
        for(uint32_t iy = 0; iy < height; iy++){
            for(uint32_t ix = 0; ix < width; ix++){
                at(x + ix, y + iy) = color;
            }
        }
    }

    void FrameBuffer::putText(std::string text,uint32_t x,uint32_t y,const sFONT& font,Pixel color){
        for(size_t idx = 0; idx < text.size();idx++){
            putChar(text[idx] ,x + (idx * font.Width) ,y ,font,color);
        }
    }

    void FrameBuffer::putChar(uint8_t character,uint32_t x,uint32_t y,const sFONT& font,Pixel color){
        const size_t widthBytes = ((font.Width - 1) / 8) + 1;
        const size_t charBytes = widthBytes * font.Height;

        const uint8_t * const charAddress = &font.table[charBytes * (character - ' ')];

        constexpr uint32_t BYTE_BIT_COUNT = 8;
        
        for(uint32_t idx = 0;idx < charBytes;idx++){
            for(uint32_t bitIdx = 0; bitIdx < BYTE_BIT_COUNT;bitIdx++){
                bool curBit = (0x80 >> bitIdx) & charAddress[idx];

                if(curBit == true){
                    size_t pixelX = (((idx % widthBytes) * 8) + bitIdx);
                    at(x + pixelX,y + (idx / widthBytes)) = color;
                }
            }
        }
    }
}

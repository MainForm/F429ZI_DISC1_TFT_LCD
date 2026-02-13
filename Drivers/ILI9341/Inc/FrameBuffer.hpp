
#ifdef __cplusplus

#ifndef __FRAMEBUFFER_LIB_H__
#define __FRAMEBUFFER_LIB_H__

#include <cstdint>
#include <string>
#include "font/fonts.hpp"

namespace TFT_LCD {
    union Pixel{
        uint16_t value;

        struct {
            uint16_t blue  : 5;
            uint16_t green : 6;
            uint16_t red   : 5;
        } color;

        Pixel() : value{0} {}
        Pixel(uint16_t val) : value{val} {}
        Pixel(uint8_t blue, uint8_t green, uint8_t red) :
            color{.blue = blue, .green = green, .red = red} {}

        operator uint16_t() const {
            return value;
        }
    };

    class FrameBuffer{
    public:
        enum PixelFormat : uint8_t{
            RGB565  = 2
        };

    private:
        Pixel* _buffer = nullptr;
        uint32_t _width = 0;
        uint32_t _height = 0;
        uint32_t _pixelFormat = 0;
        
    public:
        FrameBuffer() = default;
        explicit FrameBuffer(uint16_t* const buffer, uint32_t width, uint32_t height,PixelFormat format = RGB565);

        FrameBuffer& operator=(const FrameBuffer& other);

        void setBuffer(Pixel* const newBuffer);

        void* getBufferAddress() const {
            return _buffer;
        }

        Pixel& at(uint32_t x, uint32_t y);
        uint32_t getWidth() const;

        void setWidth(uint32_t width);

        uint32_t getHeight() const;

        void setHeight(uint32_t height);

        void drawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Pixel color);

        void putText(std::string text, uint32_t x,uint32_t y,const sFONT& font,Pixel color);
        void putChar(uint8_t character,uint32_t x,uint32_t y,const sFONT& font,Pixel color);
    };
}

#endif // __FRAMEBUFFER_LIB_H__

#endif // __cplusplus
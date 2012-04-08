#ifndef _ARDUINO3D_FRAMEBUFFER_HPP_
#define _ARDUINO3D_FRAMEBUFFER_HPP_

#include <stdint.h>

namespace A3D
{

/// 1 bit per pixel framebuffer
class Framebuffer
{
    public:
        typedef uint8_t FBDimensionType;
        typedef uint16_t FBLengthType;

        static const FBLengthType kSize = 128 * 64/8; ///< @bug hard-coded framebuffer length
        FBDimensionType width;
        FBDimensionType height;

    public:
        Framebuffer(FBDimensionType width_, FBDimensionType height_);

        virtual void init();

        virtual void flush();

        virtual void clear(uint8_t filler = 0x0);

        /// Bresenham's line drawing algorithm
        virtual void line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool color);

        uint8_t data[kSize];
};
} // namespace A3D
#endif

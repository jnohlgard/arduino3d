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

        /// copy a block into framebuffer at arbitrary non divide-by-8 y position
        virtual void blit(uint8_t x0, uint8_t y0, const uint8_t buffer_width, const uint8_t buffer_height, const uint8_t* buf);

        uint8_t data[kSize];
};
} // namespace A3D
#endif

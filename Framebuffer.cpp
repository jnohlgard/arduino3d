#include "Framebuffer.hpp"
#ifndef abs
#define abs(x) ((x)>0?(x):-(x))
#endif

namespace A3D
{
Framebuffer::Framebuffer(FBDimensionType width_, FBDimensionType height_) :
            width(width_),
            height(height_)
{
}

void Framebuffer::init()
{
}

void Framebuffer::flush()
{
}

void Framebuffer::clear(uint8_t filler)
{
    for (FBLengthType i = 0; i < kSize; ++i)
    {
        data[i] = filler;
    }
}

/// Bresenham's line drawing algorithm
void Framebuffer::line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool color)
{
    int8_t dx = abs(x2 - x1);
    int8_t dy = abs(y2 - y1);
    //~ if (dx == 1 && dy == 1)
    //~ {
        //~ /// \bug ugly hack fix for too small e2 and err variable
        //~ ++dx;
    //~ }

    int8_t sx = 1; // Step direction
    int8_t sy = 1; // Step direction
    if (x1 > x2)
    {
        sx = -1;
    }
    if (y1 > y2)
    {
        sy = -1;
    }

    int16_t err = dx-dy; // Error counter
    uint8_t x = x1;
    uint8_t y = y1;
    uint8_t* pixel = &data[y/8*width + x];
    uint8_t by = y % 8; // sub-byte (bit) position
    uint8_t bitmask = 1<<by;
    //~ Serial.print("x1 = ");
    //~ Serial.print(x1);
    //~ Serial.print(" x2 = ");
    //~ Serial.print(x2);
    //~ Serial.print(" y1 = ");
    //~ Serial.print(y1);
    //~ Serial.print(" y2 = ");
    //~ Serial.println(y2);
    while (x != x2 || y != y2)
    {
        if (y < 0 || x < 0 || y >= height || x >= width)
        {
            // Outside device!
            break;
        }
        //~ Serial.print("x = ");
        //~ Serial.print(x);
        //~ Serial.print(" y = ");
        //~ Serial.print(y);
        pixel = &data[y/8*width + x];
        by = y % 8; // sub-byte (bit) position
        bitmask = 1<<by;
        *pixel |= bitmask;
        int16_t e2 = 2*err;
        //~ Serial.print(" err = ");
        //~ Serial.print(err);
        //~ Serial.print(" e2 = ");
        //~ Serial.println(e2);
        if (e2 > -dy)
        {
            err -= dy;
            x   += sx;
            //~ pixel += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y   += sy;
        }

        //~ uint8_t By = y1/8; // Byte position, 1 byte per 8 Y values
        //~ uint8_t by = y1%8; // sub-byte (bit) position
        //~ uint8_t Bx = x2; // Byte position, 1 byte per 1 X value
                         //~ // No sub-byte position in X
        //~ uint8_t bitmask = 1<<by;
    }
    if (color)
    {
        // inverted
    }
}
} // namespace A3D

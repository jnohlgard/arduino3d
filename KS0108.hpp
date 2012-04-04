#ifndef _ARDUINO3D_KS0108_HPP_
#define _ARDUINO3D_KS0108_HPP_

#include "Framebuffer.hpp"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


namespace Framebuffer
{
class KS0108Framebuffer : public Framebuffer
{
    private:
        volatile uint8_t* const control_port;
        volatile uint8_t* const data_port;
        volatile uint8_t* const data_direction;
        // Useful combinations
        const uint8_t data_high_mask;
        const uint8_t data_low_mask;
        const uint8_t wait_high_mask;
        const uint8_t wait_low_mask;
        //~ const uint8_t cmd_high_mask;
        const uint8_t cmd_low_mask;
        const uint8_t pin_E_mask;
        const uint8_t cs_all_mask;
        const uint8_t cs_off_mask;
        const uint8_t cs_1_mask;
        const uint8_t cs_2_mask;
        // Flags, see data sheet
        static const uint8_t kBusyFlagMask = B10000000;

        static const uint8_t chipWidth = 64;
        static const uint8_t pageHeight = 8;
        static const uint8_t chipHeight = 64/pageHeight;

        /// \todo make this non-static somehow
        // All control pins _must_ be attached to the same hardware port register (for performance reasons)
        static const uint8_t kPinE = 37;
        static const uint8_t kPinCS1 = 32;
        static const uint8_t kPinCS2 = 33;
        static const uint8_t kPinRW = 35;
        static const uint8_t kPinRS = 34;
        static const uint8_t kPinData = 22; // This can be any data pin, only the hardware port matters. Data pins are assumed to be in hardware order.

        // Commands from data sheet.
        // all togglable bits are zeroed (OR this with the data)
        static const uint8_t kCmdOnOff = B00111110;
        static const uint8_t kCmdSetAddress = B01000000;
        static const uint8_t kCmdSetPage = B10111000;
        static const uint8_t kCmdSetStartLine = B11000000;

        void dataInput();
        void dataOutput();

        void clockE();

        void selectChip1();
        void selectChip2();
        void selectChipBoth();

        /// Busy wait for ready bit
        void waitReady();

        void modeCmd();
        void modeData();

        /// Write memory byte
        void writeData(uint8_t val);
        void writeCmd(uint8_t val);

        void cmdBoth(uint8_t cmd);

        /// Set page on both chips
        void setPage(uint8_t page);

        /// Set address on both chips
        void setAddress(uint8_t address);

        /// Set start line on both chips
        void setStartLine(uint8_t line);
        /// Set display on or off
        void setOn(uint8_t on);

    public:
        KS0108Framebuffer(FBDimensionType width_, FBDimensionType height_);

        void init();
        void flush();
};
} // namespace Framebuffer
#endif

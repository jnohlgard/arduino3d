#include "KS0108.hpp"

namespace A3D
{
KS0108Framebuffer::KS0108Framebuffer(FBDimensionType width_, FBDimensionType height_) :
    Framebuffer(width_, height_),
    control_port(portOutputRegister(digitalPinToPort(kPinE))),
    data_port(portOutputRegister(digitalPinToPort(kPinData))),
    data_direction(portModeRegister(digitalPinToPort(kPinData))),
    data_high_mask(digitalPinToBitMask(kPinRS)),
    data_low_mask(~digitalPinToBitMask(kPinRW)),
    wait_high_mask(digitalPinToBitMask(kPinRW)),
    wait_low_mask(~digitalPinToBitMask(kPinRS)),
    cmd_low_mask(~(digitalPinToBitMask(kPinRW) | digitalPinToBitMask(kPinRS))),
    pin_E_mask(digitalPinToBitMask(kPinE)),
    cs_all_mask(digitalPinToBitMask(kPinCS1) | digitalPinToBitMask(kPinCS2)),
    cs_off_mask(~(digitalPinToBitMask(kPinCS1) | digitalPinToBitMask(kPinCS2))),
    cs_1_mask(digitalPinToBitMask(kPinCS1)),
    cs_2_mask(digitalPinToBitMask(kPinCS2))
{
}

inline void KS0108Framebuffer::dataInput()
{
    *data_direction = 0x00; // Data port as input
}

inline void KS0108Framebuffer::dataOutput()
{
    *data_direction = 0xFF; // Data port as output
}

inline void KS0108Framebuffer::clockE()
{
    //~ delayMicroseconds(1); // tC in data sheet, E cycle time, is minimum 1000ns = 1µs
    // need to wait for tWL ns before letting pin E go high
    // tWL in data sheet is minimum 450ns, that is 3 HW instructions-ish
    asm volatile("nop\nnop\nnop":::);
    *control_port |= pin_E_mask;
    // need to wait for tWH ns before letting pin E go low again.
    // tWH in data sheet is minimum 450ns, that is 3 HW instructions-ish
    asm volatile("nop\nnop\nnop":::);
    //~ delayMicroseconds(1);
    *control_port &= ~pin_E_mask;
}

inline void KS0108Framebuffer::selectChip1()
{
    *control_port &= cs_off_mask;
    *control_port |= cs_1_mask;
}

inline void KS0108Framebuffer::selectChip2()
{
    *control_port &= cs_off_mask;
    *control_port |= cs_2_mask;
}

/// Busy wait for ready bit
void KS0108Framebuffer::waitReady()
{
    *data_port = 0x00;
    dataInput(); // Read data port
    *control_port &= wait_low_mask;
    *control_port |= wait_high_mask;
    clockE();
    while (*data_port & kBusyFlagMask)
    {
        // Do nothing, busy wait.
        /// \todo Add interrupt handler
    }
    dataOutput(); // data port is now an output
}

inline void KS0108Framebuffer::modeCmd()
{
    dataOutput();
    *control_port &= cmd_low_mask;
    //~ *control_port |= highmask;
}

inline void KS0108Framebuffer::modeData()
{
    dataOutput();
    *control_port &= data_low_mask;
    *control_port |= data_high_mask;
}

/// Write memory byte
void KS0108Framebuffer::writeData(uint8_t val)
{
    modeData();
    *data_port = val;
    clockE();
}

/// Write memory byte
void KS0108Framebuffer::writeCmd(uint8_t val)
{
    modeCmd();
    *data_port = val;
    clockE();
}

/// Set page on both chips
inline void KS0108Framebuffer::setPage(uint8_t page)
{
    writeCmd(kCmdSetPage | (page & B00000111));
}

/// Set address on both chips
inline void KS0108Framebuffer::setAddress(uint8_t address)
{
    writeCmd(kCmdSetAddress | (address & B00111111));
}

/// Set start line on both chips
inline void KS0108Framebuffer::setStartLine(uint8_t line)
{
    writeCmd(kCmdSetStartLine | (line & B00111111));
}

inline void KS0108Framebuffer::setOn(uint8_t on)
{
    writeCmd(kCmdOnOff | on);
}

void KS0108Framebuffer::init()
{
    pinMode(kPinE, OUTPUT);
    pinMode(kPinCS1, OUTPUT);
    pinMode(kPinCS2, OUTPUT);
    pinMode(kPinRW, OUTPUT);
    pinMode(kPinRS, OUTPUT);

    // digitalWrite is slow but we only do this once, the data transfers are optimized in writeData().
    digitalWrite(kPinE, LOW);

    selectChip1();
    waitReady();
    setStartLine(0);
    waitReady();
    setPage(0);
    waitReady();
    setAddress(0);
    waitReady();
    selectChip2();
    waitReady();
    setStartLine(0);
    waitReady();
    setPage(0);
    waitReady();
    setAddress(0);
    waitReady();

    // Turn on display
    selectChip1();
    waitReady();
    setOn(1);
    waitReady();
    selectChip2();
    waitReady();
    setOn(1);
    waitReady();
}

void KS0108Framebuffer::flush()
{
    // Data is split into pages, one page is 64 x 8 bits
    // Each byte represents 8 rows and 1 column on the display

    // We will write 8 lines at a time.
    uint8_t* data_ptr = &data[0];
    setStartLine(0);
    for (uint8_t page = 0; page < chipHeight; ++page)
    {
        selectChip1();
        waitReady();
        setPage(page);
        waitReady();
        setAddress(0);
        waitReady();
        for (uint8_t column = 0; column < chipWidth; ++column)
        {
            waitReady();
            writeData(*data_ptr);
            ++data_ptr;
            //~ delay(10); // for debugging
        }
        selectChip2();
        waitReady();
        setPage(page);
        waitReady();
        setAddress(0);
        waitReady();
        for (uint8_t column = 0; column < chipWidth; ++column)
        {
            waitReady();
            writeData(*data_ptr);
            ++data_ptr;
        }
        waitReady();
    }
}
} // namespace A3D

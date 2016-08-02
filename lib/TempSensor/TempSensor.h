#ifndef __TempSensor_h__
#define __TempSensor_h__

#include <Arduino.h>

class TempSensor {
    public:
        TempSensor(int pin);
        bool sample();
        float read();
    private:
        int _pin;
        bool reset();
        bool writeOne();
        void writeZero();
        void writeByte(uint8_t b);
        uint8_t readByte();
		uint8_t crc8(const uint8_t *addr, uint8_t len);
};

#endif

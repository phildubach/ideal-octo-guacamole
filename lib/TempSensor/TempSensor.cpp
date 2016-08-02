#include <Arduino.h>
#include <TempSensor.h>

#define PIN_HIGH(pin) pinMode(pin, INPUT_PULLUP)
#define PIN_LOW(pin)  digitalWrite(pin, LOW); pinMode(pin, OUTPUT)
#define PIN_READ(pin) digitalRead(pin)

TempSensor::TempSensor(int pin) {
    _pin = pin;
}

bool TempSensor::reset() {
    int ret;
    PIN_LOW(_pin);
    delayMicroseconds(500);
    PIN_HIGH(_pin);
    delayMicroseconds(100);
    ret = PIN_READ(_pin);
    delayMicroseconds(400);
    return !ret;
}

bool TempSensor::writeOne() {
    bool ret;
    PIN_LOW(_pin);
    delayMicroseconds(2);
    PIN_HIGH(_pin);
    delayMicroseconds(5);
    ret = PIN_READ(_pin);
    delayMicroseconds(55);
    return ret;
}

void TempSensor::writeZero() {
    PIN_LOW(_pin);
    delayMicroseconds(60);
    PIN_HIGH(_pin);
    delayMicroseconds(2);
}

void TempSensor::writeByte(uint8_t b) {
    uint8_t mask;
    for (mask = 1; mask != 0; mask <<= 1) {
        if (b & mask) {
            writeOne();
        } else {
            writeZero();
        }
    }
}

uint8_t TempSensor::readByte()
{
    uint8_t mask;
    uint8_t ret = 0;
    for (mask = 1; mask != 0; mask <<= 1) {
        if (writeOne()) {
            ret |= mask;
        }
    }
    //Serial.print("rx:"); Serial.println(ret, HEX);
    return ret;
}

bool TempSensor::sample() {
    bool ret = reset();
    writeByte(0xCC); // skip ROM
    writeByte(0x44); // start conversion
    return ret;
}

float TempSensor::read() {
    if (readByte() == 0xFF) // conversion done
    {
        uint8_t buf[9];
        // read value
        reset();
        writeByte(0xCC); // skip ROM
        writeByte(0xBE); // read scratchpad
        for (int i = 0; i < 9; i++)
        {
            buf[i] = readByte();
        }
        if (crc8(buf, 9) == 0) // crc okay
        {
            sample(); // start next conversion
            // store new value
            int16_t temp = (((int16_t)buf[1]) << 8) + buf[0];
            return temp / 16.0;
        }
    }
    return -127.0;
}

// calculate the checksum to verify all data was received correctly
uint8_t TempSensor::crc8(const uint8_t *addr, uint8_t len) {
    uint8_t crc = 0;

    while (len--) {
        uint8_t inbyte = *addr++;
        for (uint8_t i = 8; i; i--) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}

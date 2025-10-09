#pragma once
#include <heltec.h>

struct CONSOMMATION {
    byte bytes[2] = {0};

    CONSOMMATION(byte b0, byte b1) {
        this->bytes[0] = b0;
        this->bytes[1] = b1;
    }
    CONSOMMATION(byte bytes[]) {
        memcpy(this->bytes, bytes, 2);
    }
    CONSOMMATION(float value) {
        uint16_t intValue = static_cast<uint16_t>(round(value));
        this->bytes[0] = (intValue >> 8) & 0xFF;
        this->bytes[1] = intValue & 0xFF;
    }

    float toFloat() const {
        uint16_t intValue = bytes[0] << 8 | bytes[1];
        return intValue;
    }
};
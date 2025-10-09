#ifndef RADIO_H
#define RADIO_H

#include <heltec.h>
#include <RadioLib.h>
#include "conf.h"

struct NetworkID {
    NetworkID(uint8_t b0=0, uint8_t b1=0, uint8_t b2=0, uint8_t b3=0) {
        bytes[0] = b0; bytes[1] = b1; bytes[2] = b2; bytes[3] = b3;
    }

    NetworkID(uint8_t networkId[]) {
        memcpy(bytes, networkId, 4);
    }

    uint8_t bytes[4] = {0};

    int32_t toInt32() {
        return *((int32_t *)bytes);
    }

    bool isBroadcast() {
        return toInt32() == 0xFFFFFFFF;
    }
};

class Radio {

    public:
        Radio(NetworkID _networkId);
        void init();
        int16_t startReceive();
        int16_t receive(uint8_t data[], size_t len);
        int16_t readData(uint8_t data[], size_t len);
        int16_t sendData(byte data[], size_t length);

        int16_t sendFrisquetData(uint8_t idDestination, uint8_t idExpediteur, uint8_t idAssociation, uint8_t rollingCode, byte data[], size_t length);
        
        size_t getPacketLength();
        void onReceive(void (*func)());

        NetworkID getNetworkId();
        void setNetworkId(NetworkID networkId);

    private:
        SX1262 radio;
        NetworkID networkId;
};

#endif
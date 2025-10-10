#pragma once

#include "heltec.h"
#include "radio.h"
#include "mqtt.h"

#define ID_FRISQUET_CONNECT     0x7E
#define ID_CHAUDIERE            0x80
#define ID_SONDE_EXTERIEURE     0x20

struct ASSOCIATION_INFOS {
    NetworkID networkId = {0xFF,0xFF,0xFF,0xFF};
    uint8_t associationId = 0x00;
};

class Device {
    public:
        int16_t sendData(uint8_t idDestination, byte donnees[], size_t length);
        int16_t receiveData(uint8_t idDestination, byte donnees[], size_t *length);        
        int16_t waitingAnswer(uint8_t idDestination, byte donnees[], size_t *length, uint32_t timeout = 200);
       
        uint8_t getRollingCode();
        void setRollingCode(uint8_t rollingCode);
        uint8_t incrementRollingCode();
        bool isReady();
        
    protected:
        Device(Radio *radio, MQTT *mqtt, uint8_t idExpediteur, uint8_t idAssociation);
        static ASSOCIATION_INFOS associer(Radio *radio, uint8_t idAppareil);

        void sendPayload(byte byteArr[], size_t len);

        Radio *radio;
        MQTT *mqtt;
    private:
        uint8_t idAssociation = 0;
        uint8_t idExpediteur = 0;
        uint8_t rollingCode = 0;
};
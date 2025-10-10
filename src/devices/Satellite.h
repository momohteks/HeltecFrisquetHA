#pragma once

#include "heltec.h"
#include "radio.h"
#include "Device.h"
#include "../types/Temperature.h"

class Satellite: public Device {
    public:
        Satellite(Radio *radio, MQTT *mqtt, uint8_t idZone, uint8_t idAssociation);
        
        bool onReceive(byte donnees[], size_t length);

        static ASSOCIATION_INFOS associer(Radio *radio, uint8_t idZone);
};
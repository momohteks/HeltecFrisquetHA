#pragma once

#include "heltec.h"
#include "radio.h"
#include "Device.h"
#include "../types/Temperature.h"

struct TEMPERATURE_EXTERIEURE_TRAME {
    byte cmd[11] = {0x01, 0x17, 0x9c, 0x54, 0x00, 0x04, 0xa0, 0x29, 0x00, 0x01, 0x02};
    TEMPERATURE16 temperatureExterieure = 0.0f;
};

class SondeExterieure: public Device {
    public:
        SondeExterieure(Radio *radio, MQTT *mqtt, uint8_t idAssociation);

        bool envoyerTemperature(float temperature);

        ASSOCIATION_INFOS associer(Radio *radio);
};
#pragma once

#include "heltec.h"
#include "radio.h"
#include "Device.h"
#include "../types/Temperature.h"
#include "../trames.h"

class SondeExterieure: public Device {
    public:
        SondeExterieure(Radio *radio, MQTT *mqtt, uint8_t idAssociation);

        bool envoyerTemperature(float temperature);

        static ASSOCIATION_INFOS associer(Radio *radio);
};
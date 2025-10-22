#pragma once

#include "heltec.h"
#include "radio.h"
#include "Device.h"
#include "../types/Temperature.h"
#include "../trames.h"

class Satellite: public Device {
    public:
        Satellite(Radio *radio, MQTT *mqtt, uint8_t idZone, uint8_t idAssociation);
        
        bool onReceive(byte donnees[], size_t length);

        static ASSOCIATION_INFOS associer(Radio *radio, uint8_t idZone);

        void setTemperatureConsigne(float temperature);
        float getTemperatureConsigne();

        void setTemperatureAmbiance(float temperature);
        float getTemperatureAmbiance();
        
        uint8_t getIdZone();
    
    private:
        float temperatureConsigne = 0;
        float temperatureAmbiance = 0;
        uint8_t mode = 0x00;
};
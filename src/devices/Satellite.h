#pragma once

#include "heltec.h"
#include "radio.h"
#include "Device.h"
#include "../types/Temperature.h"
#include "../trames.h"
#include "../types/Zone.h"

class Satellite: public Device {
    public:
        enum MODE_SATELLITE : uint8_t {
            REDUIT_PERMANENT = 0x00,
            CONFORT_PERMANENT = 0x01,
            CONFORT_AUTO = 0x05,
            REDUIT_AUTO = 0x04,
            CONFORT_DEROGATION = 0x03,
            REDUIT_DEROGATION = 0x02,
            HORS_GEL = 0x10
        };

        Satellite(Radio *radio, MQTT *mqtt, uint8_t idZone, uint8_t idAssociation);
        
        bool onReceive(byte donnees[], size_t length);

        static ASSOCIATION_INFOS associer(Radio *radio, uint8_t idZone);

        void setTemperatureConsigne(float temperature);
        float getTemperatureConsigne();

        void setTemperatureAmbiance(float temperature);
        float getTemperatureAmbiance();
        
        uint8_t getIdZone();

        bool envoyerZone(MODE_ZONE mode, float temperatureConfort, float temperatureReduit, float temperatureHorsGel);

        int16_t sendData(uint8_t idDestination, uint8_t idExpediteur, byte donnees[], size_t length);
    
    private:
        float temperatureConsigne = 0;
        float temperatureAmbiance = 0;
        uint8_t mode = 0x00;
};
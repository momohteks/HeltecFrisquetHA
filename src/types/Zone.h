#pragma once

#include <heltec.h>
#include "Temperature.h"
#include "../trames.h"

#define MODEOPT_BOOST 0x40

#define ID_ZONE_1 0x08
#define ID_ZONE_2 0x09
#define ID_ZONE_3 0x0a

enum MODE_ZONE : uint8_t {
    INCONNU = 0x00,
    AUTO = 0x05,
    CONFORT = 0x06,
    REDUIT = 0X07,
    HORS_GEL = 0x08
};

class Zone {
    public:
        Zone();
        Zone(const ZONE_TRAME* trame);

        void setTemperatureConfort(float temperature);
        void setTemperatureReduit(float temperature);
        void setTemperatureHorsGel(float temperature);

        float getTemperatureConfort();
        float getTemperatureReduit();
        float getTemperatureHorsGel();

        void enableBoost();
        void disableBoost();
        bool isBoostEnable();

        uint8_t getIdZone();

        MODE_ZONE getMode();
        void setMode(MODE_ZONE mode);
        void setMode(const char* mode);

        String getNomMode();

        void fromBytes(const ZONE_TRAME* trame);
        void toBytes(ZONE_TRAME* trame);

    private:
        uint8_t idZone = 0x00;
        float temperatureConfort;                       // Début 5°C -> max 30°C
        float temperatureReduit;                        // Début 5°C -> max 30°C
        float temperatureHorsGel;                       // Début 5°C -> max 30°C
        MODE_ZONE mode = MODE_ZONE::INCONNU;            // 0x05 auto - 0x06 confort - 0x07 reduit - 0x08 hors gel
        byte modeOptions = 0x05;
        /*
        Mode Option structure bits
            inconnu1: 1 bit,
            boost: 1 bit,
            inconnu2: 2 bits,
            inconnu3: 2 bits,
            derogation: 1 bit,
            confort: 1 bit
        */

        byte dimanche[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        byte lundi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        byte mardi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        byte mercredi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        byte jeudi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        byte vendredi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        byte samedi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};
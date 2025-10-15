#pragma once

#include <heltec.h>

struct ZONE_TRAME {
    byte idZone = 0x00;
    byte code = 0x17; // Lecture + écriture
    byte cmd1[4] = {0xA1, 0x54, 0x00, 0x15}; // Zone mémoire lecture
    byte cmd2[4] = {0xA1, 0x54, 0x00, 0x18}; // Zone mémoire écriture
    uint8_t length = 0x30; // Longueur des données
    TEMPERATURE8 temperatureConfort;    // Début 5°C -> 0 = 50 = 5°C - MAX 30°C
    TEMPERATURE8 temperatureReduit;     // Début 5°C -> 0 = 50 = 5°C - MAX Confort
    TEMPERATURE8 temperatureHorsGel;     // Début 5°C -> 0 = 50 = 5°C - MAX Hors gel
    uint8_t mode = 0x00;                       // 0x05 auto - 0x06 confort - 0x07 reduit - 0x08 hors gel
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
    byte inconnu = 0x00;
    byte dimanche[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte lundi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte mardi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte mercredi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte jeudi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte vendredi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte samedi[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};


struct TEMPERATURES_TRAME {
    TEMPERATURE16 temperatureECS;
    TEMPERATURE16 temperatureCDC;
    TEMPERATURE16 temperatureDepartZ1;
    TEMPERATURE16 temperatureDepartZ2;
    TEMPERATURE16 temperatureDepartZ3;
    byte i2[26] = {0};
    TEMPERATURE16 temperatureAmbianteZ1;
    TEMPERATURE16 temperatureAmbianteZ2;
    TEMPERATURE16 temperatureAmbianteZ3;
    byte i3[6] = {0};
    TEMPERATURE16 temperatureConsigneZ1;
    TEMPERATURE16 temperatureConsigneZ2;
    TEMPERATURE16 temperatureConsigneZ3;
    TEMPERATURE16 temperatureExterieure;
};


struct TEMPERATURE_EXTERIEURE_TRAME {
    byte cmd[11] = {0x01, 0x17, 0x9c, 0x54, 0x00, 0x04, 0xa0, 0x29, 0x00, 0x01, 0x02};
    TEMPERATURE16 temperatureExterieure = 0.0f;
};

struct SATELLITE_ENVOI_CONSIGNE_TRAME {
    uint8_t cmd[11] = { 0x01, 0x17, 0xA0, 0x29, 0x00, 0x15, 0xA0, 0x2F, 0x00, 0x04, 0x08 };
    TEMPERATURE16 temperatureAmbiante; 
    TEMPERATURE16 temperatureConsigne;
    uint8_t i1 = 0x00; 
    uint8_t mode = 0x00; // 0x01 Confort, 0x02 Reduit
    uint8_t i2[2] = {0};

    SATELLITE_ENVOI_CONSIGNE_TRAME(uint8_t idZone) {
        if(idZone == 0x08) { // Zone 1
            cmd[7] == 0x2F;
        } else if(idZone == 0x09) { // Zone 2
            cmd[7] == 0x34;
            i1 = 0x80; // ? valeur proprre au module hydraulique ?
        }
    }
};
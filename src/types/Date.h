#pragma once
#include <heltec.h>

struct Date {
    uint8_t annee = 0;
    uint8_t mois = 0;
    uint8_t jour = 0;
    uint8_t heure = 0;
    uint8_t minute = 0;
    uint8_t seconde = 0;

    Date() {}
    Date(byte donnees[6]) {
        annee = ((donnees[0] >> 4) * 10) + (donnees[0] & 0x0F);
        mois = ((donnees[1] >> 4) * 10) + (donnees[1] & 0x0F);
        jour = ((donnees[2] >> 4) * 10) + (donnees[2] & 0x0F);
        heure = ((donnees[3] >> 4) * 10) + (donnees[3] & 0x0F);
        minute = ((donnees[4] >> 4) * 10) + (donnees[4] & 0x0F);
        seconde = ((donnees[5] >> 4) * 10) + (donnees[5] & 0x0F);
    }

    uint32_t toTime() const {
        uint16_t fullYear = 2000 + annee;
        uint32_t jours = 0;

        // Ajoute les jours des années précédentes
        for (uint16_t y = 2000; y < fullYear; y++) {
            jours += (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 366 : 365;
        }

        // Ajoute les jours des mois précédents
        static const uint8_t jpm[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
        for (uint8_t m = 1; m < mois; m++) {
            jours += jpm[m - 1];
            if (m == 2 && (fullYear % 4 == 0 && (fullYear % 100 != 0 || fullYear % 400 == 0)))
                jours++; // février bissextile
        }

        // Ajoute les jours du mois courant
        jours += (jour - 1);

        // Convertit tout en secondes
        uint32_t secondes = (jours * 86400UL)
                        + (heure * 3600UL)
                        + (minute * 60UL)
                        + seconde;

        return secondes;
    }
};
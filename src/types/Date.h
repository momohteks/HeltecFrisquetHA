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

    uint32_t toTime() {
        return annee * mois * jour * heure * minute * seconde;
    }
};
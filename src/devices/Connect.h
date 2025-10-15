#pragma once

#include "heltec.h"
#include "radio.h"
#include "mqtt.h"
#include "Device.h"
#include "../types/Zone.h"
#include "../types/Temperature.h"
#include "../types/ConsommationGaz.h"
#include "../trames.h"

class Connect: public Device {
    public:
        Connect(Radio *radio, MQTT* mqtt, uint8_t idAssociation);
        bool onReceive(byte donnees[], size_t length);

        bool envoyerZone(Zone *zone);
        bool recupererTemperatures();
        bool recupererConsommationGaz();
        bool recupererDate(Date *date);
        bool recupererVacances();
        bool recupererPlanning();
        bool envoyerConsigne(uint8_t idZone, float tempAmbiante, float tempConsigne, MODE_ZONE mode, bool confort = false, bool derogation = false);

        Zone* getZone1();
        Zone* getZone2();
        Zone* getZone3();

        // Consommation gaz
        float getConsommationGazECS();
        float getConsommationGazChauffage();

        // Températures
        float getTemperatureECS();
        float getTemperatureCDC();
        float getTemperatureDepartZ1();
        float getTemperatureDepartZ2();
        float getTemperatureDepartZ3();

        float getTemperatureAmbianteZ1();
        float getTemperatureAmbianteZ2();
        float getTemperatureAmbianteZ3();
       
        float getTemperatureConsigneZ1();
        float getTemperatureConsigneZ2();
        float getTemperatureConsigneZ3();

        float getTemperatureExterieure();

        // Association
        static ASSOCIATION_INFOS associer(Radio *radio);

        bool envoiModeZ1();
        bool envoiModeZ2();
        bool envoiModeZ3();

        void verifierBoost();
        bool activerBoost(Zone* zone);
        bool desactiverBoost(Zone* zone);
        
    private: 
        Zone zone1;
        Zone zone2;
        Zone zone3;

        float temperatureECS;
        float temperatureCDC;
        float temperatureDepartZ1;
        float temperatureDepartZ2;
        float temperatureDepartZ3;
        float temperatureAmbianteZ1;
        float temperatureAmbianteZ2;
        float temperatureAmbianteZ3;
        float temperatureConsigneZ1;
        float temperatureConsigneZ2;
        float temperatureConsigneZ3;
        float temperatureExterieure;
        
        float consommationGazECS = 0;
        float consommationGazChauffage = 0;
        
        bool _envoiModeZ1 = false;
        bool _envoiModeZ2 = false;
        bool _envoiModeZ3 = false;
};
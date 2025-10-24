#pragma once

#include "heltec.h"
#include "radio.h"
#include "mqtt.h"
#include "Device.h"
#include "../types/Zone.h"
#include "../types/Temperature.h"
#include "../types/ConsommationGaz.h"
#include "../trames.h"
#include "../Timer.h"

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

        void initConfig();
        void saveConfig();

        // Association
        static ASSOCIATION_INFOS associer(Radio *radio);

        Timer envoiModeZ1();
        Timer envoiModeZ2();
        Timer envoiModeZ3();

        void verifierBoost();
        bool activerBoost(Zone* zone);
        bool desactiverBoost(Zone* zone);
        
    private: 
        Zone zone1;
        Zone zone2;
        Zone zone3;

        float temperatureECS = NAN;
        float temperatureCDC = NAN;
        float temperatureDepartZ1 = NAN;
        float temperatureDepartZ2 = NAN;
        float temperatureDepartZ3 = NAN;
        float temperatureAmbianteZ1 = NAN;
        float temperatureAmbianteZ2 = NAN;
        float temperatureAmbianteZ3 = NAN;
        float temperatureConsigneZ1 = NAN;
        float temperatureConsigneZ2 = NAN;
        float temperatureConsigneZ3 = NAN;
        float temperatureExterieure = NAN;
        
        float consommationGazECS = NAN;
        float consommationGazChauffage = NAN;
        
        Timer _envoiModeZ1 = Timer(15000);
        Timer _envoiModeZ2 = Timer(15000);
        Timer _envoiModeZ3 = Timer(15000);
};
#include "Satellite.h"
#include "../types/Zone.h"

Satellite::Satellite(Radio *radio, MQTT *mqtt, uint8_t idZone, uint8_t idAssociation) : Device(radio, mqtt, idZone, idAssociation) {
}

ASSOCIATION_INFOS Satellite::associer(Radio *radio, uint8_t idZone) {
    return Device::associer(radio, idZone);
}

void Satellite::setTemperatureConsigne(float temperature) { 
    this->temperatureConsigne = temperature;
}
float Satellite::getTemperatureConsigne() {
    return this->temperatureConsigne;
}

void Satellite::setTemperatureAmbiance(float temperature) {
    this->temperatureAmbiance = temperature;
}
float Satellite::getTemperatureAmbiance() {
    return this->temperatureAmbiance;
}

uint8_t Satellite::getIdZone() {
    return this->getIdExpediteur();
}

bool Satellite::envoyerZone(MODE_ZONE mode, float temperatureConfort, float temperatureReduit, float temperatureHorsGel) {
    if(! this->isReady()) {
        return false;
    }
    
    DBG_PRINTLN("Tentative envoi zone satellite...");

    byte requete[sizeof(ZONE_TRAME)];
    ZONE_TRAME trame;
    
    // Envoi sans planning
    trame.cmd2[3] = 0x03;
    trame.length = 6;

    trame.idZone = 0x01; // Envoi direct au satellite, pas d'intermédiaire
    trame.temperatureConfort = temperatureConfort;
    trame.temperatureReduit = temperatureReduit;
    trame.temperatureHorsGel = temperatureHorsGel;
    trame.mode = mode;

    trame.idZone = this->getIdZone();
    memcpy(requete, &trame, sizeof(ZONE_TRAME));

    uint8_t retry = 0;

    do {
        delay(100);
        this->incrementRollingCode();
        int16_t err = this->sendData(this->getIdZone(), ID_CHAUDIERE, requete, (sizeof(requete) - 48) + 6 );
        if(err != RADIOLIB_ERR_NONE) {
            continue;
        }

        byte donnees[RADIOLIB_SX126X_MAX_PACKET_LENGTH];

        size_t length = 0;
        for (uint8_t i = 0; i < 10; i++) {
            err = this->radio->receive(donnees, 0);
            if(err != RADIOLIB_ERR_NONE) {
                continue;
            }

            length = this->radio->getPacketLength();
            
            #if DEBUG
            this->sendPayload(donnees, length);
            #endif

            if( length >= 4 &&
                donnees[0] == this->getIdZone() &&
                donnees[1] == ID_CHAUDIERE &&
                donnees[3] == this->getRollingCode()) {
                continue;
            } else if( length >= 4 &&
                donnees[1] == this->getIdZone() &&
                donnees[0] == ID_CHAUDIERE &&
                donnees[3] == this->getRollingCode()) {
                return true;
            }

            break;
        }
        
    } while(retry++ < 10);

    return false;
}

bool Satellite::onReceive(byte donnees[], size_t length) {
    if(length == sizeof(INFOS_ZONES_TRAME) && 
    donnees[0] == 0x01 && donnees[1] == 0x17 && donnees[2] == 0x2A) { // Récéption trame infos Zones
        INFOS_ZONES_TRAME trame;
        memcpy(&trame, donnees, sizeof(trame));
        DBG_PRINTF("Température extérieure : %f\n", trame.temperatureExterieure.toFloat()); // Test température extérieure

        if(this->getIdZone() == ID_ZONE_1) {
            this->setTemperatureConsigne(trame.temperatureConsigne1.toFloat());
            this->setTemperatureAmbiance(trame.temperatureAmbiante1.toFloat());
        } else if(this->getIdZone() == ID_ZONE_2) {
            this->setTemperatureConsigne(trame.temperatureConsigne2.toFloat());
            this->setTemperatureAmbiance(trame.temperatureAmbiante2.toFloat());
        } if(this->getIdZone() == ID_ZONE_3) {
            this->setTemperatureConsigne(trame.temperatureConsigne3.toFloat());
            this->setTemperatureAmbiance(trame.temperatureAmbiante3.toFloat());
        }

        return true;
    }
    
    return false;
}

int16_t Satellite::sendData(uint8_t idDestination, uint8_t idExpediteur, byte donnees[], size_t length) {
    byte buff[4 + length] = {idDestination, idExpediteur, 0x00, this->getRollingCode()};
    memcpy(&buff[4], donnees, length);

   
    #if DEBUG
        //TODO A SUPPRIMER
    /*    Serial.println("Envoi !");
        Serial.println(length+4);
        for(int i = 0; i < length+4; i++) {
            Serial.print(" 0x");
            Serial.print(buff[i], HEX);
        }
        Serial.println("");*/

    sendPayload(buff, sizeof(buff));
    #endif
    
   return this->radio->sendData(buff, 4+length);
}
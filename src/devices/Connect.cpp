#include "Connect.h"

Connect::Connect(Radio *radio, MQTT *mqtt, uint8_t idAssociation) :  Device(radio, mqtt, ID_FRISQUET_CONNECT, idAssociation) {}


bool Connect::envoyerZone(Zone *zone) {
    if(! this->isReady()) {
        return false;
    }
    
    if(this->zone1.getMode() == MODE_ZONE::INCONNU) { // Si aucun mode encore récupéré (initialisation)
        return false;
    }

    byte requete[sizeof(ZONE_TRAME)];
    ZONE_TRAME trame;
    zone->toBytes(&trame);
    memcpy(requete, &trame, sizeof(ZONE_TRAME));

    uint8_t retry = 0;

    if(zone->getIdZone() == ID_ZONE_1) {
        this->_envoiModeZ1 = true;
    } else if(zone->getIdZone() == ID_ZONE_2) {
        this->_envoiModeZ2 = true;
    } if(zone->getIdZone() == ID_ZONE_3) {
        this->_envoiModeZ3 = true;
    }

    do {
        delay(100);
        this->incrementRollingCode();
        int16_t err = this->sendData(ID_CHAUDIERE, requete, sizeof(requete));
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
                donnees[0] == zone->getIdZone() &&
                donnees[1] == ID_CHAUDIERE &&
                donnees[3] == this->getRollingCode()) {
                continue;
            } else if( length >= 4 &&
                donnees[1] == zone->getIdZone() &&
                donnees[0] == ID_CHAUDIERE &&
                donnees[3] == this->getRollingCode()) {
                continue;
            } else if( length >= 4 &&
                donnees[0] == ID_FRISQUET_CONNECT &&
                donnees[1] == ID_CHAUDIERE) {
                    
                if(zone->getIdZone() == ID_ZONE_1) {
                    this->_envoiModeZ1 = false;
                } else if(zone->getIdZone() == ID_ZONE_2) {
                    this->_envoiModeZ2 = false;
                } if(zone->getIdZone() == ID_ZONE_3) {
                    this->_envoiModeZ3 = false;
                }
                
                return true;
            }

            break;
        }
        
    } while(retry++ < 3);

    return false;
}

bool Connect::recupererTemperatures() {
    if(! this->isReady()) {
        return false;
    }

    byte requete[] = {0x01, 0x03, 0x79, 0xE0, 0x00, 0x1c};

    uint8_t retry = 0;

    do {
        delay(500);
        this->incrementRollingCode();
        uint8_t err = this->sendData(ID_CHAUDIERE, requete, sizeof(requete));
        if (err != RADIOLIB_ERR_NONE) {
            continue;
        }

        byte donnees[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
        size_t length = 0;
        err = this->waitingAnswer(ID_CHAUDIERE, donnees, &length);

        if(err != RADIOLIB_ERR_NONE) {
            continue;
        }

        const byte codeReception[] = {0x81, 0x03, 0x38}; // C1, C2, Taille
        if (length==59 && 
            donnees[0] == codeReception[0] && 
            donnees[1] == codeReception[1] && 
            donnees[2] == codeReception[2]) {
            TEMPERATURES_TRAME temperatures;
            memcpy(&temperatures, &donnees[3], sizeof(TEMPERATURES_TRAME));
            
            this->temperatureAmbianteZ1 = temperatures.temperatureAmbianteZ1.toFloat();
            this->temperatureAmbianteZ2 = temperatures.temperatureAmbianteZ2.toFloat();
            this->temperatureAmbianteZ3 = temperatures.temperatureAmbianteZ3.toFloat();

            this->temperatureConsigneZ1 = temperatures.temperatureConsigneZ1.toFloat();
            this->temperatureConsigneZ2 = temperatures.temperatureConsigneZ2.toFloat();
            this->temperatureConsigneZ3 = temperatures.temperatureConsigneZ3.toFloat();

            this->temperatureDepartZ1 = temperatures.temperatureDepartZ1.toFloat();
            this->temperatureDepartZ2 = temperatures.temperatureDepartZ2.toFloat();
            this->temperatureDepartZ3 = temperatures.temperatureDepartZ3.toFloat();

            this->temperatureCDC = temperatures.temperatureCDC.toFloat();
            this->temperatureECS = temperatures.temperatureECS.toFloat();

            this->temperatureExterieure = temperatures.temperatureExterieure.toFloat();

            return true;
        }
    } while(retry++ < 5);

    return false;
}

bool Connect::recupererConsommationGaz() {
    if(! this->isReady()) {
        return false;
    }

    byte requete[] = {0x01, 0x03, 0x7A, 0x18, 0x00, 0x1C};
    uint8_t retry = 0;

    do {
        delay(500);
        this->incrementRollingCode();
        uint8_t err = this->sendData(ID_CHAUDIERE, requete, sizeof(requete));
        if (err != RADIOLIB_ERR_NONE) {
            continue;
        }

        byte donnees[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
        size_t length = 0;
        err = this->waitingAnswer(ID_CHAUDIERE, donnees, &length);
        if(err != RADIOLIB_ERR_NONE) {
            delay(500);
            continue;
        }

        const byte codeReception[] = {0x81, 0x03, 0x38};
        if (length==59 && 
            donnees[0] == codeReception[0] && 
            donnees[1] == codeReception[1] && 
            donnees[2] == codeReception[2]) {
            this->consommationGazECS = CONSOMMATION(&donnees[21]).toFloat();
            this->consommationGazChauffage = CONSOMMATION(&donnees[23]).toFloat();
            return true;
        }
    } while(retry++ < 5);

    return false;
}

bool Connect::recupererDate(Date* date) {
    if(! this->isReady()) {
        return false;
    }

    byte requete[] = {0x01, 0x03, 0xa0, 0x2b, 0x00, 0x04};
    uint8_t retry = 0;

    do {
        delay(500);
        this->incrementRollingCode();
        uint8_t err = this->sendData(ID_CHAUDIERE, requete, sizeof(requete));
        if (err != RADIOLIB_ERR_NONE) {
            continue;
        }

        byte donnees[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
        size_t length = 0;
        err = this->waitingAnswer(ID_CHAUDIERE, donnees, &length);
        if(err != RADIOLIB_ERR_NONE) {
            continue;
        }

        const byte codeReception[] = {0x81, 0x03, 0x08};
        if (length==11 && 
            donnees[0] == codeReception[0] && 
            donnees[1] == codeReception[1] && 
            donnees[2] == codeReception[2]) {
            *date = &donnees[3];
            return true;
        }

        return true;

    } while(retry++ < 5);

    return false;
}

bool Connect::recupererVacances() {
    if(! this->isReady()) {
        return false;
    }

    byte requete[] = {0x01, 0x17, 0xa0, 0xf0, 0x00, 0x15, 0x9c, 0x40, 0x00, 0x01, 0x02, 0x00, 0x00};
    
    uint8_t retry = 0;

    do {
        delay(500);
        this->incrementRollingCode();
        uint8_t err = this->sendData(ID_CHAUDIERE, requete, sizeof(requete));
        if (err != RADIOLIB_ERR_NONE) {
            continue;
        }

        byte donnees[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
        size_t length = 0;
        err = this->waitingAnswer(ID_CHAUDIERE, donnees, &length);

        if(err != RADIOLIB_ERR_NONE) {
            continue;
        }
        return true;
        delay(500);
    } while(retry++ < 5);

    return false;
}

bool Connect::recupererPlanning() {
    if(! this->isReady()) {
        return false;
    }
    
    byte requete[] = {0x01, 0x03, 0xA1, 0x53, 0x00, 0x1C};
    
    uint8_t retry = 0;

    do {
        delay(500);
        this->incrementRollingCode();
        uint8_t err = this->sendData(ID_CHAUDIERE, requete, sizeof(requete));
        if (err != RADIOLIB_ERR_NONE) {
            continue;
        }

        byte donnees[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
        size_t length = 0;
        err = this->waitingAnswer(ID_CHAUDIERE, donnees, &length);

        if(err != RADIOLIB_ERR_NONE) {
            continue;
        }
        return true;
        delay(500);
    } while(retry++ < 5);

    return false;
}

ASSOCIATION_INFOS Connect::associer(Radio *radio) {
    return Device::associer(radio, ID_FRISQUET_CONNECT);
}

// GETTERS

float Connect::getConsommationGazECS() {
    return this->consommationGazECS;
}
float Connect::getConsommationGazChauffage() {
    return this->consommationGazChauffage;
}

Zone* Connect::getZone1() {
    return &this->zone1;
}
Zone* Connect::getZone2() {
    return &this->zone2;
}
Zone* Connect::getZone3() {
    return &this->zone3;
}


// EVENTS

bool Connect::onReceive(byte donnees[], size_t length) {
    if(! this->isReady()) {
        return false;
    }

    if (length == 59) {
        if(donnees[1] == 0x17) { // Récupération zone
            DBG_PRINTLN("Récupération Zone"); 
            if(donnees[0] == ID_ZONE_1 && !this->envoiModeZ1()) {
                zone1.fromBytes((ZONE_TRAME*)donnees);
            } else if(donnees[0] == ID_ZONE_2 && !this->envoiModeZ2()) {
                zone2.fromBytes((ZONE_TRAME*)donnees);
            } else if(donnees[0] == ID_ZONE_3 && !this->envoiModeZ3()) {
                zone3.fromBytes((ZONE_TRAME*)donnees);
            }

            this->sendData(ID_CHAUDIERE, donnees, length);
            return true;
        }
    }

    return false;
}

bool Connect::envoiModeZ1() {
    return this->_envoiModeZ1;
}

bool Connect::envoiModeZ2() {
    return this->_envoiModeZ2;
}

bool Connect::envoiModeZ3() {
    return this->_envoiModeZ3;
}



// ============= Températures =============

float Connect::getTemperatureECS() {
    return this->temperatureECS;
}
float Connect::getTemperatureCDC() {
    return this->temperatureCDC;
}
float Connect::getTemperatureDepartZ1() {
    return this->temperatureDepartZ1;
}
float Connect::getTemperatureDepartZ2() {
    return this->temperatureDepartZ2;
}
float Connect::getTemperatureDepartZ3() {
    return this->temperatureDepartZ3;
}

float Connect::getTemperatureAmbianteZ1() {
    return this->temperatureAmbianteZ1;
}
float Connect::getTemperatureAmbianteZ2() {
    return this->temperatureAmbianteZ2;
}
float Connect::getTemperatureAmbianteZ3() {
    return this->temperatureAmbianteZ3;
}

float Connect::getTemperatureConsigneZ1() {
    return this->temperatureConsigneZ1;
}
float Connect::getTemperatureConsigneZ2() {
    return this->temperatureConsigneZ2;
}
float Connect::getTemperatureConsigneZ3() {
    return this->temperatureConsigneZ3;
}

float Connect::getTemperatureExterieure() {
    return this->temperatureExterieure;
}

bool Connect::activerBoost(Zone* zone) {
    Date dateActuelle;

    if(!zone->boostActif() && this->recupererDate(&dateActuelle)) {
        zone->activerBoost(dateActuelle);
        this->envoyerZone(zone);
        return true;
    }

    return false;
}

bool Connect::desactiverBoost(Zone* zone) {
    if(zone->boostActif()) {
        zone->desactiverBoost();
        this->envoyerZone(zone);
        return true;
    }

    return false;
}


void Connect::verifierBoost() {
    if(this->zone1.boostActif() || this->zone2.boostActif() || this->zone2.boostActif()) {
        Date dateActuelle;
        if(this->recupererDate(&dateActuelle)) {
            if(this->zone1.boostActif() && this->zone1.verifierBoost(dateActuelle)) {
                this->zone1.desactiverBoost();
                this->envoyerZone(&this->zone1);
            }
            if(this->zone2.boostActif() && this->zone2.verifierBoost(dateActuelle)) {
                this->zone2.desactiverBoost();
                this->envoyerZone(&this->zone2);
            }
            if(this->zone3.boostActif() && this->zone3.verifierBoost(dateActuelle)) {
                this->zone3.desactiverBoost();
                this->envoyerZone(&this->zone3);
            }
        }
    }
}
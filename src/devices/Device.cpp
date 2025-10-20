#include "Device.h"

Device::Device(Radio *radio, MQTT *mqtt, uint8_t idExpediteur, uint8_t idAssociation) : 
    radio(radio), 
    mqtt(mqtt),
    idExpediteur(idExpediteur), 
    idAssociation(idAssociation) {}

int16_t Device::sendData(uint8_t idDestination, byte donnees[], size_t length) {
    byte buff[4 + length] = {idDestination, this->idExpediteur, this->idAssociation, this->getRollingCode()};
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

int16_t Device::receiveData(uint8_t idDestination, byte* donnees, size_t *length) {
    byte buff[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
    int16_t err = this->radio->receive(buff, 0);
    
    if (err != RADIOLIB_ERR_NONE) {
        return err;
    }

    size_t len = this->radio->getPacketLength();
    if (buff[0] != this->idExpediteur || 
        buff[1] != idDestination || 
        buff[2] != this->idAssociation) {
            return RADIOLIB_ERR_INVALID_PAYLOAD;
    }

    *length = len-4;
    memcpy(donnees, &buff[4], *length);
    return err;
}

void Device::sendPayload(byte byteArr[], size_t len) {  
    const char hexDigits[] = "0123456789ABCDEF";
    char message[255];
    message[0] = '\0';
    int pos = 0;

    // Convertir chaque octet en deux caractères hexadécimaux
    for (int i = 0; i < len; i++) {
        uint8_t b = byteArr[i];
        // Premier nibble (4 bits)
        message[pos++] = hexDigits[b >> 4];
        // Deuxième nibble
        message[pos++] = hexDigits[b & 0x0F];
        // Ajouter un espace séparateur
        message[pos++] = ' ';
        // Vérifier qu'on ne dépasse pas la taille du buffer
        if (pos >= (int)(sizeof(message) - 2))
        break;
    }

    // Si on a ajouté un espace en trop à la fin, on l'enlève
    if (pos > 0 && message[pos - 1] == ' ') {
        pos--;
    }

    // Terminer la chaîne
    message[pos] = '\0';
    this->mqtt->publish("homeassistant/sensor/frisquet/payload/state", message);
}

int16_t Device::waitingAnswer(uint8_t idDestination, byte* donnees, size_t* length, uint32_t timeout) {
    unsigned long startTime = millis();  // Début du chronomètre
    
    do {
        byte buff[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
        int16_t err = this->radio->receive(buff, 0);

        // Si pas de trame reçue pour l’instant, on continue (RadioLib peut retourner timeout local)
        if (err == RADIOLIB_ERR_RX_TIMEOUT) {
            // Petit yield pour laisser respirer la loop (sinon 100% CPU)
            delay(5);
            continue;
        }

        // Si une autre erreur sérieuse est survenue, on sort
        if (err != RADIOLIB_ERR_NONE) {
            continue;
        }

        // On a une trame valide
        size_t len = this->radio->getPacketLength();

        #if DEBUG
        // Debug
     /*   Serial.println(F("Réponse !"));
        Serial.print(F("Longueur : ")); Serial.println(len);
        for (size_t i = 0; i < len; i++) {
            Serial.print(F(" 0x"));
            Serial.print(buff[i], HEX);
        }
        Serial.println();*/
        
        sendPayload(buff, len);
        #endif

        // Vérification de l’en-tête
        if (len < 4) continue; // protection buffer

        if (buff[0] != this->idExpediteur || 
            buff[1] != idDestination || 
            buff[2] != this->idAssociation || 
            buff[3] != this->getRollingCode()) {
            continue; // Mauvaise trame, on attend la suivante
        }

        // Extraction du payload
        *length = len - 4;
        memcpy(donnees, &buff[4], *length);
        return RADIOLIB_ERR_NONE; // Succès
        
    } while (millis() - startTime < timeout);

    // Timeout écoulé sans trame valide
    return RADIOLIB_ERR_RX_TIMEOUT;
}


uint8_t Device::getRollingCode() {
    return this->rollingCode;
}

void Device::setRollingCode(uint8_t rollingCode) {
    this->rollingCode = rollingCode;
}

uint8_t Device::incrementRollingCode() {
    if(this->rollingCode == 0xFF) {
        this->rollingCode = 0;
    }
    return ++this->rollingCode;
}

ASSOCIATION_INFOS Device::associer(Radio *radio, uint8_t idAppareil) {
    DBG_PRINT(F("En attente d'association...\r"));

    NetworkID oldNetworkId = radio->getNetworkId();
    radio->setNetworkId(NetworkID({0xFF,0xFF,0xFF,0xFF}));

    ASSOCIATION_INFOS associationInfos;

    byte requete[10] = {ID_CHAUDIERE, idAppareil, 0x00, 0x00, 0x82, 0x41, 0x01, 0x21, 0x01, 0x02};

    uint8_t retry= 0;
    do {
        byte donnees[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
        int16_t state = radio->receive(donnees, 0);
        if (state != RADIOLIB_ERR_NONE) {
            continue;
        }
        size_t len = radio->getPacketLength();
        if (len != 11) {
            continue;
        }

        requete[2] = donnees[2];
        requete[3] = donnees[3];
        requete[4] = donnees[4] | 0x80;
        requete[5] = donnees[5];
        requete[6] = donnees[6];

        uint8_t _retry = 0;
        do {
            // Envoi de la chaine d'association
            state = radio->sendData(requete, sizeof(requete));
            if (state != RADIOLIB_ERR_NONE) {
                continue;
            }

            associationInfos.networkId = &donnees[len - 4];
            associationInfos.associationId = donnees[2];

            DBG_PRINTLN(F("Association effectuée !"));
            radio->setNetworkId(associationInfos.networkId);
            radio->init();

            return associationInfos;
        } while(_retry++ < 5);

    } while (retry++ < 20);

    radio->setNetworkId(oldNetworkId);
    return associationInfos;
}

bool Device::isReady() {
    return this->idExpediteur != 0 && this->idAssociation != 0xFF;
}
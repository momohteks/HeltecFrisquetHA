#include "Satellite.h"

Satellite::Satellite(Radio *radio, MQTT *mqtt, uint8_t idZone, uint8_t idAssociation) : Device(radio, mqtt, idZone, idAssociation) {
}

ASSOCIATION_INFOS Satellite::associer(Radio *radio, uint8_t idZone) {
    return Device::associer(radio, idZone);
}

bool Satellite::onReceive(byte donnees[], size_t length) {
    return true;
}
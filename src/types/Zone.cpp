#include "Zone.h"

Zone::Zone() {}
Zone::Zone(uint8_t idZone) {
    this->idZone = idZone;
}
Zone::Zone(const ZONE_TRAME* trame) {
    this->fromBytes(trame);
}

void Zone::fromBytes(const ZONE_TRAME* trame) {
    this->idZone = trame->idZone;
    this->temperatureConfort = trame->temperatureConfort.toFloat();
    this->temperatureReduit = trame->temperatureReduit.toFloat();
    this->temperatureHorsGel = trame->temperatureHorsGel.toFloat();
    this->mode = MODE_ZONE(trame->mode);
    this->modeOptions = trame->modeOptions;

    if(this->boostActif()) {
        this->temperatureConfort -= BOOST_TEMPERATURE;
    }

    memcpy(this->lundi, trame->lundi, sizeof(this->lundi));
    memcpy(this->mardi, trame->mardi, sizeof(this->mardi));
    memcpy(this->mercredi, trame->mercredi, sizeof(this->mercredi));
    memcpy(this->jeudi, trame->jeudi, sizeof(this->jeudi));
    memcpy(this->vendredi, trame->vendredi, sizeof(this->vendredi));
    memcpy(this->samedi, trame->samedi, sizeof(this->samedi));
    memcpy(this->dimanche, trame->dimanche, sizeof(this->dimanche));
}

void Zone::toBytes(ZONE_TRAME* trame) {
    trame->idZone = this->idZone;
    trame->temperatureConfort = this->temperatureConfort;
    trame->temperatureReduit = this->temperatureReduit;
    trame->temperatureHorsGel = this->temperatureHorsGel;
    trame->mode = this->mode;
    trame->modeOptions = this->modeOptions;

    if(this->boostActif()) {
        trame->temperatureConfort = this->temperatureConfort + BOOST_TEMPERATURE;
    }

    memcpy(trame->lundi, this->lundi, sizeof(this->lundi));
    memcpy(trame->mardi, this->mardi, sizeof(this->mardi));
    memcpy(trame->mercredi, this->mercredi, sizeof(this->mercredi));
    memcpy(trame->jeudi, this->jeudi, sizeof(this->jeudi));
    memcpy(trame->vendredi, this->vendredi, sizeof(this->vendredi));
    memcpy(trame->samedi, this->samedi, sizeof(this->samedi));
    memcpy(trame->dimanche, this->dimanche, sizeof(this->dimanche));
}

void Zone::setTemperatureConfort(float temperature, bool force) {
    float temperatureAdapte = std::min(30.0f, std::max(this->temperatureReduit, temperature));
    if(force) {
        temperatureAdapte = std::max(5.0f, std::min(30.0f, temperature));
    }
    this->temperatureConfort = temperatureAdapte;
}
void Zone::setTemperatureReduit(float temperature, bool force) {
    float temperatureAdapte = std::min(this->temperatureConfort, std::max(this->temperatureHorsGel, temperature));
    if(force) {
        temperatureAdapte = std::max(5.0f, std::min(30.0f, temperature));
    }
    this->temperatureReduit = temperatureAdapte;
}
void Zone::setTemperatureHorsGel(float temperature, bool force) {
    float temperatureAdapte = std::min(this->temperatureReduit, std::max(5.0f, temperature));
    if(force) {
        temperatureAdapte = std::max(5.0f, std::min(30.0f, temperature));
    }
    this->temperatureHorsGel = temperatureAdapte;
}

float Zone::getTemperatureConfort() {
    return this->temperatureConfort;
}
float Zone::getTemperatureReduit() {
    return this->temperatureReduit;
}
float Zone::getTemperatureHorsGel() {
    return this->temperatureHorsGel;
}

MODE_ZONE Zone::getMode() {
    return (MODE_ZONE)this->mode;
}

void Zone::setMode(MODE_ZONE mode) {
    this->mode = mode;
}
void Zone::setMode(const String& mode) {
    if (mode.equalsIgnoreCase("Auto")) {
    this->setMode(MODE_ZONE::AUTO);
  } else if (mode.equalsIgnoreCase("Réduit")) {
    this->setMode(MODE_ZONE::REDUIT);
  } else if (mode.equalsIgnoreCase("Hors gel")) {
    this->setMode(MODE_ZONE::HORS_GEL);
  } else if (mode.equalsIgnoreCase("Confort")) {
    this->setMode(MODE_ZONE::CONFORT);
  }
}

String Zone::getNomMode() {
    switch(this->getMode()) {
        case MODE_ZONE::AUTO:
            return "Auto";
            break;
        case MODE_ZONE::CONFORT:
            return "Confort";
            break;
        case MODE_ZONE::REDUIT:
            return "Réduit";
            break;
        case MODE_ZONE::HORS_GEL:
            return "Hors Gel";
            break;
    }

    return "Inconnu";
}

uint8_t Zone::getIdZone() {
    return this->idZone;
}

void Zone::activerBoost(Date dateActuelle) {
    this->modeOptions |= MODEOPT_BOOST;
    this->debutBoost = dateActuelle;
}
void Zone::desactiverBoost() {
    this->modeOptions &= ~MODEOPT_BOOST;
}
bool Zone::boostActif() {
    return (this->modeOptions & MODEOPT_BOOST) != 0;    
}

bool Zone::verifierBoost(Date dateActuelle) {
    if(this->boostActif()) {
        if((this->debutBoost.toTime() + DUREE_BOOST) <= dateActuelle.toTime()) {
            return true;
        }
    }

    return false;
}
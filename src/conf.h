// conf.h
#ifndef CONF_H
#define CONF_H

#include <heltec.h>

#define DEBUG 1
#if DEBUG
#define DBG_PRINT(x) Serial.print(x)
#define DBG_PRINTF(x, y) Serial.printf(x, y)
#define DBG_PRINTLN(x) Serial.println(x)
#else
#define DBG_PRINT(x)
#define DBG_PRINTLN(x)
#endif


// Configuration Radio
#define SS GPIO_NUM_8
#define RST_LoRa GPIO_NUM_12
#define BUSY_LoRa GPIO_NUM_13
#define DIO0 GPIO_NUM_14


#define USE_CONNECT true // Activation Connect


#define USE_ZONE_2 false // Activation Zone 2
#define USE_ZONE_3 false // Activation Zone 3


// Configuration Sonde extérieure
#define USE_SONDE_EXTERIEURE true   // Activation sonde extérieure (Virtuelle OU DS18B20)
#if USE_SONDE_EXTERIEURE
    #define USE_DS18B20 true            // Option: si true, on lit le DS18B20 local comme source de T° ext.
#endif
#define DS18B20_PIN GPIO_NUM_33     // Broche du bus OneWire (DS18B20)
#define DS18B20_OFFSET 0.0f         // Décalage de calibration si besoin (ex: +0.3°C)


// Configuration ASSOCIATION ID par défaut
#define DEFAULT_FRISQUET_CONNECT_ASSOCIATION_ID 0x00
#define DEFAULT_SONDE_EXTERIEURE_ASSOCIATION_ID 0x00
#define DEFAULT_SATELLITE_Z1_ASSOCIATION_ID 0x00
#define DEFAULT_NETWORK_ID {0x05, 0xDB, 0x46, 0x64}     // par défaut : {0xFF, 0xFF, 0xFF, 0xFF}

#endif // CONFIG_H
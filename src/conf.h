// conf.h
#ifndef CONF_H
#define CONF_H

#include <heltec.h>
#include "Logs.h"

#define DEBUG 1
#if DEBUG
#define DBG_PRINT(x) logs.addLog("INFO", x)
#define DBG_PRINTF(x, y) logs.addLogf("INFO", x, y)
#define DBG_PRINTLN(x) logs.addLog("INFO", x)
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


#define USE_ZONE_2 true // Activation Zone 2
#define USE_ZONE_3 false // Activation Zone 3


// Configuration Sonde extérieure
#define USE_SONDE_EXTERIEURE false   // Activation sonde extérieure (Virtuelle OU DS18B20)
#if USE_SONDE_EXTERIEURE
    #define USE_DS18B20 true            // Option: si true, on lit le DS18B20 local comme source de T° ext.
#endif
#define DS18B20_PIN GPIO_NUM_33     // Broche du bus OneWire (DS18B20)
#define DS18B20_OFFSET 0.0f         // Décalage de calibration si besoin (ex: +0.3°C)


// Configuration ASSOCIATION ID par défaut
#define DEFAULT_FRISQUET_CONNECT_ASSOCIATION_ID 0x45
#define DEFAULT_SONDE_EXTERIEURE_ASSOCIATION_ID 0x55
#define DEFAULT_SATELLITE_Z1_ASSOCIATION_ID 0x6F
#define DEFAULT_NETWORK_ID {0x05, 0xD6, 0x10, 0x22}     // par défaut : {0xFF, 0xFF, 0xFF, 0xFF}

#endif // CONFIG_H

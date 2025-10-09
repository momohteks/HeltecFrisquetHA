# Heltec Frisquet — ESP32 + SX1262 + MQTT + Home Assistant

> Un module **ESP32 Heltec LoRa (SX1262)** qui **écoute** et **pilote** une chaudière **Frisquet**, **publie** les infos en **MQTT**, et s’**intègre** à **Home Assistant**.

---

## TL;DR

- Lit : températures (extérieure, départ, ambiante, ECS), consignes (confort/réduit/hors-gel), modes, **conso gaz** (chauffage/ECS).  
- Pilote : **mode/consignes/boost** par zone via **MQTT**.  
- **Home Assistant MQTT Discovery** (auto-création des entités).  
- **OTA** intégré + **portail de configuration** (Wi-Fi/MQTT/IDs).  
- **Multi-zones** optionnelles (Z2/Z3), **DS18B20** (option) pour température extérieure.

---

## Sommaire

- [Fonctionnalités](#fonctionnalités)  
- [Matériel requis](#matériel-requis)

---

## Fonctionnalités

- 📡 **Radio SX1262** (carte Heltec) : écoute des trames Frisquet + envoi d’ordres.
- 💬 **MQTT** : publication d’états + réception de commandes (consignes/mode/boost).
- 🏠 **Home Assistant** : topics compatibles + **discovery**.
- 🧰 **Portail de config** : Wi-Fi, MQTT, IDs réseau/association ; stockage **NVS**.
- 🔁 **OTA** : mise à jour du firmware via réseau.
- 🌡️ **DS18B20** (option) : mesure/push de la température extérieure.

---

## Matériel requis

- 1× **Heltec ESP32 WiFi LoRa 32** (SX1262 intégré).
- *(Optionnel)* 1× **DS18B20** + résistance **4,7 kΩ**.
- **Du café**.
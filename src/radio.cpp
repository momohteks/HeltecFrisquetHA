#include "radio.h"

 Radio::Radio(NetworkID networkId) : networkId(networkId), radio(new Module(SS, DIO0, RST_LoRa, BUSY_LoRa)) {}

 void Radio::init() {
    this->radio.beginFSK();
    this->radio.setFrequency(868.96);
    this->radio.setBitRate(25.0);
    this->radio.setFrequencyDeviation(50.0);
    this->radio.setRxBandwidth(250.0);
    this->radio.setPreambleLength(4);
    this->radio.setSyncWord(this->networkId.bytes, sizeof(this->networkId));
 }

 int16_t Radio::startReceive() {
    return this->radio.startReceive();
 }

 int16_t Radio::receive(uint8_t data[], size_t len) {
    return this->radio.receive(data, len);
 }

 int16_t Radio::readData(uint8_t data[], size_t len) {
    return this->radio.readData(data, len);
 }

 int16_t Radio::sendData(byte data[], size_t length) {
    return this->radio.transmit(data, length);
 }

 size_t Radio::getPacketLength() {
    return this->radio.getPacketLength();
 }

 void Radio::onReceive(void (*func)()) {
   this->radio.setPacketReceivedAction(func);
 }

 void Radio::setNetworkId(NetworkID networkId) {
   this->networkId = networkId;
   this->radio.setSyncWord(this->networkId.bytes, sizeof(this->networkId));
 }

 NetworkID Radio::getNetworkId() {
   return this->networkId;
 }
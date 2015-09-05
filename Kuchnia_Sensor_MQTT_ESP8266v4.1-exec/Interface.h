#ifndef INTERFACE_H
#define INTERFACE_H

void sendUptime();
void addToSendBuffer(int topic, int value);
void send2Ints (int value1, int value2);
int* get2Ints();
void setMqttConfig();
void setMqttNormal();

#endif

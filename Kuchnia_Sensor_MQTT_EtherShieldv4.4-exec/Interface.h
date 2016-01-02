#ifndef INTERFACE_H
#define INTERFACE_H

void sendUptime();
void send2Ints (int value1, int value2);
int* get2Ints();
void setMqttConfig();
void setMqttNormal();
bool getDiffTime(unsigned long* timer, int diffInSec);
int* getDHT11temp();
void sendMqttData();
void registerToMqttTable(int* topic, int* value, int* flag);
void callback(char* topic, byte* payload, unsigned int length);
void SendMqtt();

#endif


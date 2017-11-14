#pragma once

#define ID "\"room\""

#define SENSOR_DHT22
#define SENSOR_BMP180
#define SENSOR_DS18D20

#define NO_OF_TRY 6 //кол-во датчиков*2

#ifdef SENSOR_DHT22
#define DHTPIN 2 
#define DHTTYPE DHT22
#endif
#ifdef SENSOR_BMP180
#define ALTITUDE 159
#endif
#ifdef SENSOR_DS18D20
#define DS18D20PIN 3
#endif
#define DELAYSECONDS 300
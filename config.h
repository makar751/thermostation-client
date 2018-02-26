#pragma once

#define ID "\"room\""

#define SENSOR_DHT22
#define SENSOR_BMP180
#define SENSOR_DS18D20

#define WIFIRX 6
#define WIFITX 7
#define WIFIBAUDRATE 115200
#define WIFISSID "*"
#define WIFIPASSWORD "*"

#define SERVERIP "192.168.1.100"
#define SERVERURI "http://192.168.1.100/service/putdata.php"

#define NO_OF_TRY 3 //кол-во датчиков*2

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
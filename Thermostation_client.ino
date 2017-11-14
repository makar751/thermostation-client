/*
 Name:		Thermostation_client.ino
 Created:	12.11.2017 18:14:02
 Author:	allex
*/
#include <Wire.h>
#include "DHT.h"
#include "SFE_BMP180.h"
#include "OneWire.h"
#include "config.h"

DHT dht(DHTPIN, DHTTYPE);
OneWire ds(DS18D20PIN);
SFE_BMP180 pressure;

bool isSetupOk = true;
int error = 0;

bool GetDHTData(double& hum, double& temp) {
	double dht_hum = dht.readHumidity();
	double dht_temp = dht.readTemperature();
	if (isnan(dht_hum) || isnan(dht_temp)) {
		return false;
	}
	hum = dht_hum;
	temp = dht_temp;
	return true;
}

bool GetBMP180Data(double& press) {
	char status;
	double T, P, p0;
	status = pressure.startTemperature();
	if (status != 0)
	{
		delay(status);
		status = pressure.getTemperature(T);
		if (status != 0)
		{
			status = pressure.startPressure(3);
			if (status != 0)
			{
				delay(status);
				status = pressure.getPressure(P, T);
				if (status != 0)
				{
					p0 = pressure.sealevel(P, ALTITUDE); 
					press = p0* 0.750062;
					return true;

				}
			}
		}
	}
	return false;
}

bool GetDS18D20Data(double& temp) {
	byte i;
	byte type_s;
	byte data[12];
	byte addr[8];

	if (!ds.search(addr)) {
		ds.reset_search();
		delay(250);
		return false;
	}

	if (OneWire::crc8(addr, 7) != addr[7]) {
		return false;
	}

	switch (addr[0]) {
	case 0x10:
		type_s = 1;
		break;
	case 0x28:
		type_s = 0;
		break;
	case 0x22:
		type_s = 0;
		break;
	default:
		return false;
	}

	ds.reset();
	ds.select(addr);
	ds.write(0x44, 1);        

	delay(1000);     

	ds.reset();
	ds.select(addr);
	ds.write(0xBE);        

	for (i = 0; i < 9; i++) {          
		data[i] = ds.read();
	}

	int16_t raw = (data[1] << 8) | data[0];
	if (type_s) {
		raw = raw << 3; 
		if (data[7] == 0x10) {
			raw = (raw & 0xFFF0) + 12 - data[6];
		}
	}
	else {
		byte cfg = (data[4] & 0x60);
		if (cfg == 0x00) raw = raw & ~7;  
		else if (cfg == 0x20) raw = raw & ~3; 
		else if (cfg == 0x40) raw = raw & ~1; 									  
	}
	temp = (float)raw / 16.0;
	ds.reset_search();
	delay(250);
	return true;
}
void setup() {
	//WIFI setup will be here
	dht.begin();
	if (!pressure.begin())
		isSetupOk = false;
}

void loop() {
	if ((!isSetupOk) && (error > NO_OF_TRY)) {
		Serial.println("Error try to reboot!");
		asm volatile ("  jmp 0"); //пробуем перезагрузиться
	}
	int current_error = error;
	//пауза
	for (int i = 0; i < DELAYSECONDS; i++)
		delay(1000);
#ifdef SENSOR_DHT22
	double dht_hum = 0, dht_temp = 0;

	if (!GetDHTData(dht_hum, dht_temp))
		error++;
#endif
#ifdef SENSOR_BMP180
	double bmp180_press = 0;

	if (!GetBMP180Data(bmp180_press))
		error++;
#endif
#ifdef SENSOR_DS18D20
	double ds18d20_temp = 0;

	if (!GetDS18D20Data(ds18d20_temp))
		error++;
#endif
	String json_answ;
	json_answ = "{\"id\":";
	json_answ += ID;

	String data;
#ifdef SENSOR_DHT22
	json_answ += ",";
	data = String(dht_hum);
	json_answ += "\"hu\":\"";
	json_answ += data;
	json_answ += "\"";

	json_answ += ",";
	data = String(dht_temp);
	json_answ += "\"in\":\"";
	json_answ += data;
	json_answ += "\"";
#endif
#ifdef SENSOR_BMP180
	json_answ += ",";
	data = String(bmp180_press);
	json_answ += "\"pr\":\"";
	json_answ += data;
	json_answ += "\"";
#endif
#ifdef SENSOR_DS18D20
	json_answ += ",";
	data = String(ds18d20_temp);
	json_answ += "\"ot\":\"";
	json_answ += data;
	json_answ += "\"";
#endif
	json_answ += "}";

	Serial.println(json_answ);
	if (current_error == error)
		error = 0;
}

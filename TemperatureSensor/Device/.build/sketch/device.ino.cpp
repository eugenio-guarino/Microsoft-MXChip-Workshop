#include <Arduino.h>
#line 1 "c:\\Users\\Eugen\\Desktop\\Iot stuff\\TemperatureSensor\\Device\\device.ino"
#line 1 "c:\\Users\\Eugen\\Desktop\\Iot stuff\\TemperatureSensor\\Device\\device.ino"
#include "AZ3166WiFi.h"
#include "DevKitMQTTClient.h"
#include "Sensor.h"

DevI2C *ext_i2c;
HTS221Sensor *ht_sensor;
RGB_LED rgbLed;

static float temperatureThreshold = 25.0;

#line 11 "c:\\Users\\Eugen\\Desktop\\Iot stuff\\TemperatureSensor\\Device\\device.ino"
void initSensor();
#line 17 "c:\\Users\\Eugen\\Desktop\\Iot stuff\\TemperatureSensor\\Device\\device.ino"
float getSensorData();
#line 34 "c:\\Users\\Eugen\\Desktop\\Iot stuff\\TemperatureSensor\\Device\\device.ino"
void sendData(const char *data);
#line 48 "c:\\Users\\Eugen\\Desktop\\Iot stuff\\TemperatureSensor\\Device\\device.ino"
void setup();
#line 54 "c:\\Users\\Eugen\\Desktop\\Iot stuff\\TemperatureSensor\\Device\\device.ino"
void loop();
#line 11 "c:\\Users\\Eugen\\Desktop\\Iot stuff\\TemperatureSensor\\Device\\device.ino"
void initSensor() {
  ext_i2c = new DevI2C(D14, D15);
  ht_sensor = new HTS221Sensor(*ext_i2c);
  ht_sensor->init(NULL);
}

float getSensorData() {
  float temperature;
  ht_sensor->enable();
  ht_sensor->getTemperature(&temperature);
  ht_sensor->disable();
  ht_sensor->reset();

  char buff[16];
  sprintf(buff, "Temp:%sC\r\n", f2s(temperature, 1));
  Screen.print(1, buff);

  sprintf(buff, "Threshold:%sC\r\n", f2s(temperatureThreshold, 1));
  Screen.print(2, buff);

  return temperature;
}

void sendData(const char *data) {
  time_t t = time(NULL);
  char buf[sizeof "2011-10-08T07:07:09Z"];
  strftime(buf, sizeof buf, "%FT%TZ", gmtime(&t));

  EVENT_INSTANCE* message = DevKitMQTTClient_Event_Generate(data, MESSAGE);

  DevKitMQTTClient_Event_AddProp(message, "$$CreationTimeUtc", buf);
  DevKitMQTTClient_Event_AddProp(message, "$$MessageSchema", "temperature;v1");
  DevKitMQTTClient_Event_AddProp(message, "$$ContentType", "JSON");
  
  DevKitMQTTClient_SendEventInstance(message);
}

void setup() {
  initSensor();
  WiFi.begin();
  DevKitMQTTClient_Init(true);
}

void loop() {
  float temperature = getSensorData();

  char sensorData[400];
  sprintf(sensorData,
          "{\"temperature\":%s,\"threshold\":%s}",
          f2s(temperature, 1),
          f2s(temperatureThreshold, 1));

  sendData(sensorData);

  if (temperature > temperatureThreshold) {
    rgbLed.setColor(255, 0, 0);
  }
  else {  
    rgbLed.setColor(0, 0, 255);
  }
  
  delay(5000);
}

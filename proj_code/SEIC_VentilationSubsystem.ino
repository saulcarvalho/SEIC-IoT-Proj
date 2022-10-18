/*
   Developed by Saúl S. Carvalho

   Board: Lolin NodeMCU V3

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "bsec.h"

// WiFi Network Config
const char* ssid     = "SSID_WIFI";
const char* password = "PASS_WIFI";

// Configuração do broker MQTT
const char* mqtt_server  = "IP_SERVER_MQTT";
const int   mqttPort     = 1883;
const char* mqttUser     = "USER_SERVER_MQTT";
const char* mqttPassword = "PASS_SERVER_MQTT";

WiFiClient espClient;
PubSubClient client(espClient);
long tempInt = 0;
long fanInt  = 0;

#define ADC_VREF_mV     3300.0        // in milivolts
#define ADC_RESOLUTION  1024.0        // ADC resolution
#define PIN_LM35        A0            // GPIO36 (ADC0) -> LM35
#define PIN_FAN         16            // GPIO16        -> FAN

Bsec bme; // I2C
String output;

struct T {
  float raw = 0;
  float ftd = 25;
  float def = 25;
} temp;

struct BME {
  float temp_raw = 0;
  float temp_ftd = 0;
  float humi_raw = 0;
  float humi_ftd = 0;
  float pres_raw = 0;
  float pres_ftd = 0;
  float iaq_raw  = 0;
  float iaq_ftd  = 0;
} b;

uint8_t tempHYST_low  = 2;       // hysteresis of temperature, lower
uint8_t tempHYST_high = 2;       // hysteresis of temperature, upper

const uint8_t nSample          = 5;
float holderMean_temp[nSample] = {0};
float holderMean_humi[nSample] = {0};
float holderMean_pres[nSample] = {0};
float holderMean_iaq[nSample]  = {0};
uint8_t startSettle_counter    = 0;

bool ventiState = false;

void setup() {
  pinMode(PIN_LM35, INPUT);
  pinMode(PIN_FAN,  OUTPUT);
  digitalWrite(PIN_FAN, LOW);

  Serial.begin(115200);   // begin serial monitoring
  Wire.begin(4,5);        // SDA / SCL
  bme.begin(BME680_I2C_ADDR_SECONDARY, Wire);
  checkIaqSensorStatus();
  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };
  bme.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();

  setup_wifi();
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - tempInt > 10000) {
    tempInt = now;

    Serial.println("   ##########   RAW    ##########");
    int adcVal = analogRead(PIN_LM35);
    float mv   = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
    temp.raw = mv / 10;
    Serial.print("   > Temperatura:         ");  Serial.print(temp.raw);  Serial.println("ºC");

    char tempString[8];
    dtostrf(temp.def, 1, 2, tempString);
    client.publish("SEIC/VENTI/LM35/def", tempString);

    dtostrf(temp.raw, 1, 2, tempString);
    client.publish("SEIC/VENTI/LM35/raw", tempString);

    temp.ftd   = movMean(holderMean_temp, temp.raw,   nSample);

    bool ftd_show = false;
    if (bme.run()) {
      b.humi_raw = bme.humidity;
      b.pres_raw = bme.pressure / 100;
      b.iaq_raw  = bme.iaq;

      dtostrf(b.humi_raw, 1, 2, tempString);
      client.publish("SEIC/VENTI/SEN0248/humi/raw", tempString);
      Serial.print("   > Relative Humidity:   ");  Serial.print( b.humi_raw, 2);  Serial.println("%RH");

      dtostrf(b.pres_raw , 1, 2, tempString);
      client.publish("SEIC/VENTI/SEN0248/pres/raw", tempString);
      Serial.print("   > Atmospheric Pressure: ");  Serial.print( b.pres_raw, 2);  Serial.println("hPa");

      dtostrf(b.iaq_raw , 1, 2, tempString);
      client.publish("SEIC/VENTI/SEN0248/iaq/raw", tempString);
      Serial.print("   > IAQ: ");  Serial.println( b.iaq_raw, 1);

      b.humi_ftd = movMean(holderMean_humi, b.humi_raw, nSample);
      b.pres_ftd = movMean(holderMean_pres, b.pres_raw, nSample);
      b.iaq_ftd  = movMean(holderMean_iaq,  b.iaq_raw,  nSample);

      ftd_show = true;
    } else {
      checkIaqSensorStatus();
    }

    if ( startSettle_counter <= nSample) {
      startSettle_counter++;
      Serial.println("   > Insufficient samples to calculate moving average! Acquiring more samples ... ");
    } else {
      Serial.println("   ########## FILTERED ##########");
      Serial.print("   > Temperature:         ");  Serial.print(temp.ftd);  Serial.println("ºC   ->   Publishing in topic ...");

      char tempString[8];
      dtostrf(temp.ftd, 1, 2, tempString);
      client.publish("SEIC/VENTI/LM35/ftd", tempString);
      
      if (ftd_show) {
        Serial.print("   > Relative Humidity:   ");   Serial.print(b.humi_ftd, 2);  Serial.println("ºC   ->   Publishing in topic ...");
        Serial.print("   > Atmospheric Pressure: ");  Serial.print(b.pres_ftd, 2);  Serial.println("ºC   ->   Publishing in topic ...");
        Serial.print("   > IAQ: ");  Serial.print(b.iaq_ftd, 1);                   
        if( b.iaq_ftd <= 50) {
          Serial.print(" (GOOD)");    
        } else if( b.iaq_ftd > 50 && b.iaq_ftd <= 100) {
          Serial.print(" (AVERAGE)");    
        } else if( b.iaq_ftd > 100 && b.iaq_ftd <= 200) {
          Serial.print(" (BAD)");    
        } else if( b.iaq_ftd > 200 && b.iaq_ftd <= 300) {
          Serial.print(" (VERY BAD)");    
        } else if( b.iaq_ftd > 300 ) {
          Serial.print(" (WORST)");    
        }
        Serial.println("ºC   ->   Publishing in topic ...");
      
        dtostrf(b.humi_ftd, 1, 2, tempString);
        client.publish("SEIC/VENTI/SEN0248/humi/ftd", tempString);
        dtostrf(b.pres_ftd, 1, 2, tempString);
        client.publish("SEIC/VENTI/SEN0248/pres/ftd", tempString);
        dtostrf(b.iaq_ftd, 1, 2, tempString);
        client.publish("SEIC/VENTI/SEN0248/iaq/ftd", tempString);

        ftd_show = false;
      }
      Serial.println();
    }
  }

  now = millis();
  if (now - fanInt > 15000) {
    fanInt = now;

    if ( startSettle_counter <= nSample) {
      //
    } else {
      // ON/OFF control with hysteresis
      char tempString[8];
      uint8_t thr = (temp.def + tempHYST_high);
      dtostrf(thr, 1, 2, tempString);
      client.publish("SEIC/VENTI/upTHR", tempString);
      if (temp.ftd >= (thr)) {
        digitalWrite(PIN_FAN, HIGH);
        Serial.print("   > Temperature above ");   Serial.print(thr);  Serial.println("°C!   ->   Control output: ON");
        ventiState = true;
      }
      tempString[8];
      thr = (temp.def - tempHYST_low);
      dtostrf(thr, 1, 2, tempString);
      client.publish("SEIC/VENTI/downTHR", tempString);
      if (temp.ftd <= thr) {
        digitalWrite(PIN_FAN, LOW);
        Serial.print("   > Temperature below ");  Serial.print(thr);   Serial.println("°C!   ->   Control output: OFF");
        ventiState = false;
      }

      tempString[8];
      if (ventiState) {
        dtostrf(1, 1, 2, tempString);
      } else {
        dtostrf(0, 1, 2, tempString);
      }
      client.publish("SEIC/VENTI/state", tempString);
    }
  }
}

// Moving average function
float movMean(float * holder, float var, uint8_t nSample) {
  float sum = 0, mean = 0;

  // n sample cycle holder (shifts values to right side of array)
  for (uint8_t i = (nSample - 1); i > 0; i-- ) {
    holder[i] = holder[i - 1];
  }
  holder[0] = var; // most recent sample

  // Sum of samples
  for (uint8_t i = 0; i < nSample; i++) {
    sum = sum + holder[i];
  }
  // Moving average
  mean = sum / nSample;
  sum = 0;  // reset sum

  return mean;
}

void setup_wifi() {
  delay(10);
  Serial.println();     // Connecting to a WiFi network
  Serial.print("   > Connecting to network ");   Serial.print(ssid); Serial.print(" ");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected to WiFi network! ");
  Serial.print("   > IP Address: ");  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("   > Message arrived at topic: ");   Serial.print(topic);  Serial.print("   ->   Message: ");

  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Does something if message X arrives in topic
  if (String(topic) == "SEIC/VENTI/LM35/setDef") {
    if (messageTemp.toInt() >= 10 && messageTemp.toInt() <= 50) {
      temp.def = messageTemp.toInt();
    }
  }
  if (String(topic) == "SEIC/VENTI/LM35/setHystDown") {
    if (messageTemp.toInt() >= 1 && messageTemp.toInt() <= 10) {
      tempHYST_low = messageTemp.toInt();
    }
  }
  if (String(topic) == "SEIC/VENTI/LM35/setHystUp") {
    if (messageTemp.toInt() >= 1 && messageTemp.toInt() <= 10) {
      tempHYST_high = messageTemp.toInt();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("   > Connecting to MQTT broker...");
    if (client.connect("VentilationSubsystem", mqttUser, mqttPassword )) {     // tries to connect
      Serial.println(" Ligação ao broker bem sucedida!");
      client.subscribe("SEIC/VENTI/LM35/setDef");                              // subscribe
      client.subscribe("SEIC/VENTI/LM35/setHystDown");
      client.subscribe("SEIC/VENTI/LM35/setHystUp");
    } else {
      Serial.print(" Connection to broker failed, rc = "); Serial.print(client.state());   Serial.println(". Trying again in 5s.");
      delay(5000);
    }
  }
}

// Helper function definitions
void checkIaqSensorStatus(void) {
  if (bme.status != BSEC_OK) {
    if (bme.status < BSEC_OK) {
      output = "BSEC error code : " + String(bme.status);
      Serial.println(output);
    } else {
      output = "BSEC warning code : " + String(bme.status);
      Serial.println(output);
    }
  }

  if (bme.bme680Status != BME680_OK) {
    if (bme.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(bme.bme680Status);
      Serial.println(output);
    } else {
      output = "BME680 warning code : " + String(bme.bme680Status);
      Serial.println(output);
    }
  }
}
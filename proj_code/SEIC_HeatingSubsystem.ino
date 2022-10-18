/*
    Developed by Saúl S. Carvalho

    Board: WeMos LOLIN32 Lite

*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SdFat.h>                 // https://github.com/adafruit/SdFat

#define DEBUG                      // Output debug

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
long intTemp  = 0;
long PIDInt   = 0;
long stateInt = 0;

struct T {
  float raw = 0;
  float ftd = 25;
  float def = 25;
} temp;

struct PID {
  float     currentError  = 0;
  float     previousError = 0;

  float     pTerm  = 0;
  uint16_t  Kp     = 30;       // Gain P
  float     dTerm  = 0;
  uint16_t  Kd     = 200;      // Gain D
  float     iTerm  = 0;
  uint8_t   Ki     = 0.1;      // Gain I

  float    iSum  = 0;
  uint8_t  iMult = 0;
  float    iSumMax = 100;
  float    iSumMin = -100;

  float     out     = 0;
  uint8_t   pwm_out = 0;
} ctrl;

const int oneWireBus = 26;
OneWire oneWire(oneWireBus);            // Config of the OneWire instance
DallasTemperature sensors(&oneWire);    // Reference for the Dallas sensor

const uint8_t nSample              = 5;
float holderMean_temp[nSample + 1] = {0};
uint8_t startSettle_counter        = 0;

const int pwmPin     = 22;
const int freq       = 500;
const int pwmCH      = 0;
const int resolution = 8;

// microSD reader
SdFat sd;
SdFile myFile;
#define cs_SD 5
char fileName[13];

void setup() {
  pinMode(cs_SD, OUTPUT);
  digitalWrite(cs_SD, HIGH);
  SPI.begin(18, 19, 23, 5);
  Serial.begin(115200);   // begin serial monitoring
  sensors.begin();        // begin DS18B20 temperature sensor

  setup_wifi();
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);

  ledcSetup(pwmCH, freq, resolution);   // LED setup
  ledcAttachPin(pwmPin, pwmCH);

  sensors.requestTemperatures();        // reads first temperature
  delay(500);

  if (!sd.begin(cs_SD, SPI_HALF_SPEED)) {
    sd.initErrorHalt();                 // error msg if microSD reader fails to init
  }
  // Read configs from txt file
  // Change current directory to ROOT
  if (!sd.chdir()) {
    sd.errorHalt(" Change to root dir failed!");
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - intTemp > 10000) {
    intTemp = now;
#ifdef DEBUG
    Serial.println("   ##########   RAW    ##########");
#endif
    sensors.requestTemperatures();
    temp.raw = sensors.getTempCByIndex(0);
#ifdef DEBUG
    if ( startSettle_counter > 0) {
      Serial.print("   > Temperature: ");  Serial.print(temp.raw);  Serial.println("ºC");
    }
#endif
    delay(10);
    char tempString[8];
    dtostrf(temp.def, 1, 2, tempString);
    client.publish("SEIC/HEAT/DS18B20/def", tempString);
    delay(10);
    dtostrf(temp.raw, 1, 2, tempString);
    client.publish("SEIC/HEAT/DS18B20/raw", tempString);
    temp.ftd  = movMean(holderMean_temp, temp.raw, nSample);

    if ( startSettle_counter <= nSample) {
      startSettle_counter++;
#ifdef DEBUG
      Serial.println("   > Insufficient samples to calculate moving average! Acquiring more samples ... ");
#endif
    } else {
#ifdef DEBUG
      Serial.println("   ########## FILTERED ##########");
      Serial.print("   > Temperature: ");  Serial.print(temp.ftd);  Serial.println("ºC   ->   Publishing in topic ...");  Serial.println();
#endif
      delay(10);
      dtostrf(temp.ftd, 1, 2, tempString);
      client.publish("SEIC/HEAT/DS18B20/ftd", tempString);
    }
    //-----------------------------------------------------------------------------
    // SD create dirs and write header in raw reading day-file
    // Change current directory to ROOT
    if (!sd.chdir()) {
      sd.errorHalt(" Change to root dir failed!");
    }

    // Create nodeID dir and change to it (ROOT > nodeID)
    char folder[6] = "";
    sprintf(folder, "readings");
    if (!sd.exists(folder)) {
      if (!sd.mkdir(folder)) {
        sd.errorHalt(" Create Node dir failed!");
      }
    }
    if (!sd.chdir(folder)) {
      sd.errorHalt(" Change to Node dir failed!");
    }

    sprintf(fileName, "readFile.txt");                   // Writes the label once in the sensor file
    if (!myFile.open(fileName, O_RDWR | O_CREAT)) {      // Creates the file if it doesnt exist, otherwise nothing
      sd.errorHalt("   > Error while creating file!");
    }

    myFile.print(temp.raw, 2);   myFile.print(',');
    myFile.print(temp.ftd, 2);   myFile.print(',');
    myFile.print(temp.def, 2);   myFile.print(',');
    myFile.println();
    myFile.close();   // Closes file
  }

  if ( startSettle_counter > nSample) {

    now = millis();
    if (now - PIDInt > 100) {
      delay(10);
      // Digital PID controller
      ctrl.currentError = temp.def - temp.ftd;      // error = desired - measured
      ctrl.pTerm = ctrl.Kp * ctrl.currentError;     // Therm P
      ctrl.iSum  = ctrl.iSum + ctrl.currentError;   // Error integration

      if (ctrl.iSum > ctrl.iSumMax) {               // Anti-Windup
        ctrl.iSum = ctrl.iSumMax;
        ctrl.iMult = 0;
      } else if (ctrl.iSum < ctrl.iSumMin) {
        ctrl.iSum = ctrl.iSumMin;
        ctrl.iMult = 0;
      } else {
        ctrl.iMult = 1;
      }

      ctrl.iTerm = ctrl.Ki * ctrl.iSum * ctrl.iMult;                    // Therm I
      ctrl.dTerm = ctrl.Kd * (ctrl.currentError - ctrl.previousError);  // Therm D
      ctrl.previousError = ctrl.currentError ;
      ctrl.out = ctrl.pTerm + ctrl.iTerm + ctrl.dTerm;

      if (ctrl.out < 0) {
        ctrl.pwm_out = 0;
      } else if (ctrl.out > 255) {
        ctrl.pwm_out = 255;
      } else {
        ctrl.pwm_out = ctrl.out;
      }
      ledcWrite(pwmCH, ctrl.pwm_out);  // output PWM
    }
  }

  now = millis();
  if (now - stateInt > 1000) {
    stateInt = now;
    delay(10);
    char tempString[8];
    dtostrf(ctrl.out, 1, 2, tempString);
    client.publish("SEIC/HEAT/state", tempString);
    delay(10);
    dtostrf(ctrl.pwm_out, 1, 2, tempString);
    client.publish("SEIC/HEAT/statePWM", tempString);
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
  Serial.print("   > A ligar à rede ");   Serial.print(ssid); Serial.print(" ");

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
  if (String(topic) == "SEIC/HEAT/DS18B20/setDef") {
    if (messageTemp.toInt() >= 10 && messageTemp.toInt() <= 50) {
      temp.def = messageTemp.toInt();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("   > Connecting to MQTT broker...");
    if (client.connect("HeatingSubsystem", mqttUser, mqttPassword )) {  // tries to connect
      Serial.println(" Connection successful!");
      client.subscribe("SEIC/HEAT/DS18B20/setDef");                     // subscription
    } else {
      Serial.print(" Connection to broker failed, rc = "); Serial.print(client.state());   Serial.println(". Trying again in 5s.");
      delay(5000);
    }
  }
}

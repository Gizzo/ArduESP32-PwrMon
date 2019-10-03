#include <ArduinoJson.h>
#include <WiFi.h>
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include <DallasTemperature.h>
#include "config.h"
#include <HardwareSerial.h>
#include "time.h"

//MQTT
#include <PubSubClient.h>

#undef  MQTT_MAX_PACKET_SIZE // un-define max packet size
#define MQTT_MAX_PACKET_SIZE 500  // fix for MQTT client dropping messages over 128B

// OTA configuration
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void setup_wifi(void);

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, 5, 4);

// Secondary Serial
#define RXD0 16
#define TXD0 15

const char* ntpServer = "pool.ntp.org";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

const byte numChars = 64;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing
const char delim[2] = ",";

// variables to hold the parsed data
char cSensor[numChars] = {0};
//int integerFromPC = 0;
float rPower    = 0.0;
float aPower    = 0.0;
float pFactor   = 0.0;
float supVolts  = 0.0;
float Irms      = 0.0;

boolean newData = false;

HardwareSerial SerialAux(2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialAux.begin(115200, SERIAL_8N1, RXD0, TXD0);

  Serial.println();
  Serial.println("Serial Txd is on pin: "+String(TXD0));
  Serial.println("Serial Rxd is on pin: "+String(RXD0));
  Serial.println();

  // Initialising the UI will init the display too.
  display.init();

  // Clear the display.
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.display();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  checkWifiConnection();
  setupOTA();

  client.setServer(MQTT_SERVER, MQTT_SERVER_PORT);

  configTime(0, 0, ntpServer);

  Serial.println("Done Setup!");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Boot Complete!");
  String timeNow = String(getTime());
  display.drawString(0, 54, timeNow);

}

void loop() {

  // Create a new JSON object
  StaticJsonDocument<140> mqttPayload;

  // Connect or reconnect to MQTT server
  if (!client.connected()) {
    reconnect();
  }

  // Monitor the SerialAux input for new incomming serial data
  recvWithStartEndMarkers();

  // If there is new serial data perform the following
  if (newData == true) {
      // This temporary copy is necessary to protect the original data
      // because strtok() used in parseData() replaces the commas with \0
      strcpy(tempChars, receivedChars);

      // Parse the recieved serial data
      parseData();
      
      //Serial.print( getTime() );
      //showParsedData();

      // Build the JSON object with our sensor data
      mqttPayload["Clamp"] = cSensor;
      mqttPayload["rPower"] = rPower;
      mqttPayload["aPower"] = aPower;
      mqttPayload["pFactor"] = pFactor;
      mqttPayload["volts"] = supVolts;
      mqttPayload["Irms"] = Irms;

      // Buffer to hold serialized JSON data that was created above
      char buffer[140];

      // Build final JSON object from the data stored in mqttPayload
      size_t payloadSize = serializeJson(mqttPayload, buffer);

      // Publish the JSON object to our MQTT server
      if ( client.publish("/jkhome/PWR-Node/data", buffer, payloadSize) ) {
        Serial.print( getTime() );
        Serial.print(" - Pub sent - ");
        Serial.println(buffer);
      }
      else {
        Serial.println("Pub failed!");
      }
      
      newData = false;
  }

  //showNewData();

  client.loop();

  // Clear the display.
  display.clear();  

  display.drawString(0,0, "Time: ");
  display.drawString(55, 0, String(getTime()) );
  display.drawString(0, 15, "Votls: ");
  display.drawString(55, 15, String(supVolts) );
  
  display.display();
 
  //delay(500);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if ( client.connect( HOST ) ) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("/jkhome/PWR-Node", "PWR-Node connected...");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 2 seconds before retrying
      delay(2500);

      checkWifiConnection();
     }
  }
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (SerialAux.available() > 0 && newData == false) {
        rc = SerialAux.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//============

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(cSensor, strtokIndx); // copy it to messageFromPC
 
    //strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    //integerFromPC = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, delim);
    rPower = atof(strtokIndx);     // convert this part to a float

    strtokIndx = strtok(NULL, delim);
    aPower = atof(strtokIndx);     // convert this part to a float

    strtokIndx = strtok(NULL, delim);
    pFactor = atof(strtokIndx);     // convert this part to a float

    strtokIndx = strtok(NULL, delim);
    supVolts = atof(strtokIndx);     // convert this part to a float

    strtokIndx = strtok(NULL, delim);
    Irms = atof(strtokIndx);     // convert this part to a float

}

//============

void showParsedData() {
    Serial.print("Message rcvd = ");
    Serial.print(cSensor);
    Serial.print(", ");
    Serial.print(rPower);
    Serial.print(", ");
    Serial.print(aPower);
    Serial.print(", ");
    Serial.print(pFactor);
    Serial.print(", ");
    Serial.print(supVolts);
    Serial.print(", ");
    Serial.println(Irms);
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return(0);
  }
  
  time(&now);
  return now;
}

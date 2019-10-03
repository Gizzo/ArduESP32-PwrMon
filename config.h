/************************* WiFi Access Point ********************************/
const char* ssid = "enteryourssid";
const char* password = "enteryourpw";

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 13);    // where xx is the desired IP Address
IPAddress gateway(192, 168, 100, 254);    // set gateway to match your network
IPAddress subnet(255, 255, 255, 0);       // set subnet mask to match your network
IPAddress dns(8, 8, 8, 8);                // DNS

#define HOST  "PWR-Node"

/************************* MQTT Setup *********************************/

const char* mqttClientId = "PWR-Node";
const char* mqttUser = "";
const char* mqttPwd = "";

const char* MQTT_SERVER = "entermqttserver";
const int MQTT_SERVER_PORT = 1883;

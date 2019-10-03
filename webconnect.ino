void checkWifiConnection() {
  if (WiFi.status() != WL_CONNECTED)
  {
    int countdown = millis() + 15000;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      if (countdown > millis()) {
      delay(500);
      Serial.println("Connecting to WiFi..");
      } else {
        Serial.println("Timed out! Restarting...");
        ESP.restart();
      }
  }
  }
}

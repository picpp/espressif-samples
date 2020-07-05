#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <WebServer.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient
#include <Preferences.h>

// Callback function header
void mqttCallback(char* topic, byte* payload, unsigned int length);
void saveConfigCallback();

WiFiClient mqtt_wifi;
PubSubClient mqtt(mqtt_wifi);

// Default values, loaded from config.json
char mqtt_server[40];
char mqtt_port[6] = "1883";

// Flag for saving data
bool shouldSaveConfig = false;

void setup()
{
  // Initialize the M5StickC object
  M5.begin();

  // Internal red LED
  pinMode(M5_LED, OUTPUT);
  digitalWrite(M5_LED, HIGH);

  // LCD display
  M5.Lcd.setRotation(1);
  M5.Lcd.setCursor(8, 8);
  M5.Lcd.println("Connecting to WiFi...");

  // Wifi off
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  // Explicitly set mode, esp defaults to STA+AP
  WiFi.mode(WIFI_STA);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  // wm.setConnectTimeout(20); // how long to try to connect for before continuing
  wm.setConfigPortalTimeout(60); // auto close configportal after n seconds
  // wm.setCaptivePortalEnable(false); // disable captive portal redirection
  // wm.setAPClientCheck(true); // avoid timeout if client connected to softap
  //
  // wifi scan settings
  // wm.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
  // wm.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%
  // wm.setShowInfoErase(false);      // do not show erase button on info page
  // wm.setScanDispPerc(true);       // show RSSI as percentage not graph icons
  //
  // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", mqtt_port, 6);
  // Add all your parameters here
  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_port);
  // Set config save notify callback
  wm.setSaveConfigCallback(saveConfigCallback);

  M5.update();
  while (M5.BtnB.isPressed()) {      
    digitalWrite(M5_LED, LOW);
    delay(50);
    M5.update();
    if (M5.BtnB.pressedFor(5000)) {
      wm.resetSettings();
      Preferences prefs;
      prefs.begin("nvs", false);
      prefs.clear();
      prefs.end();
      // Blink for 2 sec.
      for (int i = 0; i < 50; i++) {
        digitalWrite(M5_LED, !digitalRead(M5_LED));
        delay(40);
      }
      //
      ESP.restart();
      delay(1000);
    }
  }

  M5.update();
  digitalWrite(M5_LED, HIGH);

  // Load parameters from NVS
  Preferences prefs;
  prefs.begin("nvs", false);
  String s_mqtt_server = prefs.getString("mqtt_server");
  String s_mqtt_port = prefs.getString("mqtt_port");
  prefs.end();
  s_mqtt_server.toCharArray(mqtt_server, 41);
  s_mqtt_port.toCharArray(mqtt_port, 7);

  if (!wm.autoConnect()) {
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  // Save parameters to NVS
  if (shouldSaveConfig) {
    // Read updated parameters
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());

    prefs.begin("nvs", false);
    prefs.putString("mqtt_server", String(mqtt_server));
    prefs.putString("mqtt_port", String(mqtt_port));
    prefs.end();

    shouldSaveConfig = false;
  }

  // Connecting to mqtt server
  mqtt.setServer(mqtt_server, atoi(mqtt_port));
  mqtt.setCallback(mqttCallback);

  delay(200);
  M5.update();

  // Clear Display
  M5.Lcd.fillScreen(TFT_BLACK);
  // Turn off LCD backlight
  M5.Axp.SetLDO2(false);
}

void loop()
{
  if (!mqtt.connected()) {
    if (mqtt.connect("M5-StickC", NULL, NULL, "/dev/m5stickc/connected", 0, false, "0")) {
      // Once connected, publish an announcement...
      mqtt.publish("/dev/m5stickc/connected", "1");
    }
  }
  mqtt.loop();

  // Button A
  if (M5.BtnA.wasPressed())
  {
    digitalWrite(M5_LED, LOW);
    if (mqtt.publish("/dev/m5stickc/buttonA", "1")) {
      delay(200);
    } else {
      // Blink for 0.5 sec.
      for (int i = 0; i < 12; i++) {
        digitalWrite(M5_LED, !digitalRead(M5_LED));
        delay(40);
      }
    }
  }
  if (M5.BtnA.wasReleased())
  {
    if (mqtt.publish("/dev/m5stickc/buttonA", "0")) {
      delay(200);
    } else {
      // Blink for 0.5 sec.
      for (int i = 0; i < 12; i++) {
        digitalWrite(M5_LED, !digitalRead(M5_LED));
        delay(40);
      }
    }
    digitalWrite(M5_LED, HIGH);
  }

  // Button B
  if (M5.BtnB.wasPressed())
  {
    digitalWrite(M5_LED, LOW);
    if (mqtt.publish("/dev/m5stickc/buttonB", "1")) {
      delay(200);
    } else {
      // Blink for 0.5 sec.
      for (int i = 0; i < 12; i++) {
        digitalWrite(M5_LED, !digitalRead(M5_LED));
        delay(40);
      }
    }
  }
  if (M5.BtnB.wasReleased())
  {
    if (mqtt.publish("/dev/m5stickc/buttonB", "0")) {
      delay(200);
    } else {
      // Blink for 0.5 sec.
      for (int i = 0; i < 12; i++) {
        digitalWrite(M5_LED, !digitalRead(M5_LED));
        delay(40);
      }
    }
    digitalWrite(M5_LED, HIGH);
  }

  delay(50);
  M5.update();
  checkAXPPress();
}

void checkAXPPress() {
  // 0x01 long press(1s), 0x02 press
  uint8_t press = M5.Axp.GetBtnPress();
  if (press == 0x01) 
  {
    // Wifi off
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    // Power off
    M5.Axp.PowerOff();
  }
  else if (press == 0x02) 
  {
    // Restart
    esp_restart();
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
}

void saveConfigCallback () {
  Serial.println("Save config");
  shouldSaveConfig = true;
}

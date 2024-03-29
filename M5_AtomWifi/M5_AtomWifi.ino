#include "M5Atom.h"
#include <WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <WebServer.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient
#include <Preferences.h>

/* Board: ESP32 Pico Kit */
/* Upload Speed: 115200 */

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

// Timeout to power off (0,5 min = 30 s)
int powerOffDelay = 30e3;
long powerOffTimeout = 0;

void setup()
{
  M5.begin(true, false, true);
  delay(10);
  M5.dis.clear();

  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__ " " __DATE__ " " __TIME__);

  // Wifi off
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  // explicitly set mode, esp defaults to STA+AP
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
  // This is sometimes necessary, it is still unknown when and why this is needed but it may solve some race condition or bug in esp SDK/lib
  // wm.setCleanConnect(true); // disconnect before connect, clean connect

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
  while (M5.Btn.isPressed()) {
    M5.dis.drawpix(0, 0xFF0000);
    delay(50);
    M5.update();
    if (M5.Btn.pressedFor(5000)) {
      M5.dis.drawpix(0, 0x00FF00);
      wm.resetSettings();
      Preferences prefs;
      prefs.begin("nvs", false);
      prefs.clear();
      prefs.end();
      //
      ESP.restart();
      delay(1000);
    }
  }

  M5.update();
  M5.dis.drawpix(0, 0x000044);

  // Load parameters from NVS
  Preferences prefs;
  prefs.begin("nvs", false);
  String s_mqtt_server = prefs.getString("mqtt_server");
  String s_mqtt_port = prefs.getString("mqtt_port");
  prefs.end();
  s_mqtt_server.toCharArray(mqtt_server, 41);
  s_mqtt_port.toCharArray(mqtt_port, 7);

  // Workarround for connection issue (WL_STATION_WRONG_PASSWORD)
  // see https://github.com/tzapu/WiFiManager/issues/979
  wm.setEnableConfigPortal(false);
  if(!wm.autoConnect()) {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    wm.setEnableConfigPortal(true);
    wm.autoConnect();
  }

  if (!wm.autoConnect()) {
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  M5.dis.drawpix(0, 0x0000FF);

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

  // Auto Power-Off
  powerOffTimeout = millis() + powerOffDelay;

  delay(200);
  M5.dis.clear();
  M5.update();
}

void loop()
{
  if (!mqtt.connected()) {
    if (mqtt.connect("ESP32_Atom", NULL, NULL, "/dev/m5atom/connected", 0, false, "0")) {
      // Once connected, publish an announcement...
      mqtt.publish("/dev/m5atom/connected", "1");
    }
  }
  mqtt.loop();

  if (M5.Btn.wasPressed())
  {
    M5.dis.drawpix(0, 0x0000FF);
    String payload = "1";
    while (M5.Btn.isPressed()) {
      delay(50);
      M5.update();
      if (M5.Btn.pressedFor(1000)) {
        payload = "2";
        break;
      }
    }
    if (mqtt.publish("/dev/m5atom/button", (char*) payload.c_str())) {
      M5.dis.drawpix(0, 0x008800);
      delay(200);
    } else {
      M5.dis.drawpix(0, 0x880000);
      delay(500);
    }
  }
  if (M5.Btn.wasReleased())
  {
    if (mqtt.publish("/dev/m5atom/button", "0")) {
      M5.dis.drawpix(0, 0x008800);
      delay(200);
    } else {
      M5.dis.drawpix(0, 0x880000);
      delay(500);
    }
    M5.dis.clear();
    // Auto Power-Off
    powerOffTimeout = millis() + powerOffDelay;
  }

  // Auto Power-off after inactivity
  if (millis() > powerOffTimeout) {
    // Flash for 0.2 sec.
    M5.dis.drawpix(0, 0x000088);
    mqtt.publish("/dev/m5atom/sleep", "");
    delay(200);
    M5.dis.clear();
    // Wifi off
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    // Configure GPIO39 as ext0 wake up source for LOW logic level
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);
    //Go to sleep now
    esp_deep_sleep_start();
  }

  delay(50);
  M5.update();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
}

void saveConfigCallback () {
  Serial.println("Save config");
  shouldSaveConfig = true;
}

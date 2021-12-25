#include "M5Atom.h"
#include <WiFi.h>

/* Board: ESP32 Pico Kit */
/* Upload Speed: 115200 */

// Timeout to power off (5 s)
int powerOffDelay = 5e3;
long powerOffTimeout = 0;

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup()
{
  M5.begin(true, false, true);
  M5.update();
  delay(10);
  M5.dis.clear();

  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__ " " __DATE__ " " __TIME__);
  print_wakeup_reason();

  // Wifi off
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // Led on
  M5.dis.drawpix(0,0x0000ff);

  // Auto Power-Off
  powerOffTimeout = millis() + powerOffDelay;
}

void loop()
{
  if (M5.Btn.wasPressed())
  {
    Serial.println("Button pressed");
    M5.dis.drawpix(0, 0xff0000);
    // Auto Power-Off
    powerOffTimeout = millis() + powerOffDelay;
  }
  if (M5.Btn.wasReleased())
  {
    Serial.println("Button released");
    M5.dis.drawpix(0, 0x00ff00);
    // Auto Power-Off
    powerOffTimeout = millis() + powerOffDelay;
  }

  // Auto Power-off after inactivity
  if (millis() > powerOffTimeout)
  {
    M5.dis.clear();
    delay(20);
    // Configure GPIO39 as ext0 wake up source for LOW logic level
    int level = 0;
    if (M5.Btn.isPressed())
      level = 1;
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, level);
    //Go to sleep now
    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
  }

  delay(50);
  M5.update();
}

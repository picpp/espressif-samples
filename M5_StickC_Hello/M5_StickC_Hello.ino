#include <M5StickC.h>

int n = 0;

// the setup routine runs once when M5StickC starts up
void setup(){
  // Initialize the M5StickC object
  M5.begin();

  // Internal red LED
  pinMode(M5_LED, OUTPUT);
  digitalWrite(M5_LED, HIGH);

  // LCD display
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextSize(2),
  M5.Lcd.println("Hello, World!");
  M5.Lcd.setTextSize(1),
  M5.Lcd.println("Press Button to change");
  M5.Lcd.println("text color");
  M5.Lcd.setTextSize(2);
}

// the loop routine runs over and over again forever
void loop() {

  if (M5.BtnA.wasReleased()) {
    n++;
    if (n>18) n = 0;
    // Change colour text
    switch (n) {
      case  0: M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK); break;
      case  1: M5.Lcd.setTextColor(TFT_NAVY, TFT_BLACK); break;
      case  2: M5.Lcd.setTextColor(TFT_DARKGREEN, TFT_BLACK); break;
      case  3: M5.Lcd.setTextColor(TFT_DARKCYAN, TFT_BLACK); break;
      case  4: M5.Lcd.setTextColor(TFT_MAROON, TFT_BLACK); break;
      case  5: M5.Lcd.setTextColor(TFT_PURPLE, TFT_BLACK); break;
      case  6: M5.Lcd.setTextColor(TFT_OLIVE, TFT_BLACK); break;
      case  7: M5.Lcd.setTextColor(TFT_LIGHTGREY, TFT_BLACK); break;
      case  8: M5.Lcd.setTextColor(TFT_DARKGREY, TFT_BLACK); break;
      case  9: M5.Lcd.setTextColor(TFT_BLUE, TFT_BLACK); break;
      case 10: M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK); break;
      case 11: M5.Lcd.setTextColor(TFT_CYAN, TFT_BLACK); break;
      case 12: M5.Lcd.setTextColor(TFT_RED, TFT_BLACK); break;
      case 13: M5.Lcd.setTextColor(TFT_MAGENTA, TFT_BLACK); break;
      case 14: M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK); break;
      case 15: M5.Lcd.setTextColor(TFT_ORANGE, TFT_BLACK); break;
      case 16: M5.Lcd.setTextColor(TFT_GREENYELLOW, TFT_BLACK); break;
      case 17: M5.Lcd.setTextColor(TFT_PINK, TFT_BLACK); break;
      case 18: M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE); break;
    }
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print("Hello, World!");
  }

  if (M5.BtnB.wasPressed()) {
    // Toggle LED state
    digitalWrite(M5_LED, !digitalRead(M5_LED));
  }


  M5.update();
  checkAXPPress();
}

void checkAXPPress()
{
  // 0x01 long press(1s), 0x02 press
  uint8_t press = M5.Axp.GetBtnPress();
  if (press == 0x01) 
  {
    // Power off
    M5.Axp.PowerOff();
  }
  else if (press == 0x02) 
  {
    // Restart
    esp_restart();
  }
}

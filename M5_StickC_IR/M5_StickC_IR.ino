#include <M5StickC.h>
#include <IRsend.h>
#include <IRutils.h>

// Set the GPIO to be used to sending the message.
#define IR_LED 9
IRsend irsend(IR_LED);

// Internal red LED
#define RED_LED_PIN 10
// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

// the setup routine runs once when M5StickC starts up
void setup() {
  // Initialize the M5StickC object
  M5.begin(false, true, true);

  // Turn off LCD backlight
  M5.Axp.SetLDO2(false);

  // Turn on internal red LED
  pinMode(RED_LED_PIN, OUTPUT);
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(RED_LED_PIN, ledChannel);
  // changing the LED brightness with PWM (0-255)
  ledcWrite(ledChannel, 0xFF<<1);

  // Begin IR
  irsend.begin();
}

// the loop routine runs over and over again forever
void loop() {

  if (M5.BtnA.isPressed()) {
    ledcWrite(ledChannel, 0xFF);
  }
  if (M5.BtnA.wasReleased()) {
    // Demo, Panasonic on/off
    uint64_t data = 0x40040100BCBD;
    irsend.sendPanasonic64(data);
    ledcWrite(ledChannel, 0x00);
    delay(50);
    ledcWrite(ledChannel, 0xFF<<1);
  }

  M5.update();
  checkAXPPress();
}

void checkAXPPress() {
  // 0x01 long press(1s), 0x02 press
  if (M5.Axp.GetBtnPress() == 0x02) {
    // Power off
    M5.Axp.PowerOff();
  }
}

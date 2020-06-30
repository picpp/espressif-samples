#include "M5Atom.h"

/* Board: ESP32 Pico Kit */

#define NUM_PIXELS 1
uint8_t DisBuff[2 + NUM_PIXELS * 3];

void setBuff(uint8_t Rdata, uint8_t Gdata, uint8_t Bdata)
{
    DisBuff[0] = 0x05;
    DisBuff[1] = 0x05;
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        DisBuff[2 + i * 3 + 0] = Rdata;
        DisBuff[2 + i * 3 + 1] = Gdata;
        DisBuff[2 + i * 3 + 2] = Bdata;
    }
}

void setup()
{
    M5.begin(true, false, true);
    delay(10);
    setBuff(0x20, 0x20, 0x20);
    M5.dis.displaybuff(DisBuff);
}

uint8_t FSM = 0;
uint8_t PV = 20;
uint8_t PD = 5;

void loop()
{
  if (FSM == 0 && PD != 0)
  {
    PV+=PD;
    if (PV < 20 || PV >= 80) {
      PD=-PD;
    }
    M5.dis.setBrightness(PV);
    delay(5);
  }

    if (M5.Btn.wasPressed())
    {

        switch (FSM)
        {
        case 0:
            setBuff(0x40, 0x00, 0x00);
            break;
        case 1:
            setBuff(0x00, 0x40, 0x00);
            break;
        case 2:
            setBuff(0x00, 0x00, 0x40);
            break;
        case 3:
            setBuff(0x20, 0x20, 0x20);
            break;
        default:
            break;
        }
        M5.dis.displaybuff(DisBuff);
        M5.dis.setBrightness(0xff);

        FSM++;
        if (FSM >= 4)
        {
            FSM = 0;
        }
        PD = 0;
    }

    delay(50);
    M5.update();
}

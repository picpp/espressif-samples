// Example: Storing struct data in RTC user rtcDataory
//
// Struct data with the maximum size of 512 bytes can be stored
// in the RTC user rtcDataory using the ESP-specifc APIs.
// The stored data can be retained between deep sleep cycles.
// However, the data might be lost after power cycling the ESP8266.

#include <Arduino.h>
extern "C" {
  #include <user_interface.h>
}

rst_info *espResetInfo;

// Structure which will be stored in RTC memory.
struct {
  uint32_t crc32;
  uint32_t counter;
} rtcData;

// CRC function used to ensure data validity
uint32_t calculateCRC32(const uint8_t *data, size_t length);

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(20);

  espResetInfo = ESP.getResetInfoPtr();
  Serial.print("ESP8266 ResetInfo->Reason ");
  Serial.println(espResetInfo->reason);

  // Read struct from RTC memory
  if (ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
    // Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
    uint32_t crc32 = calculateCRC32(((uint8_t*) &rtcData) + 4, sizeof(rtcData) - 4);
    if (crc32 != rtcData.crc32) {
      rtcData.counter = 0;
    }
  }

  Serial.print("Counter: ");
  Serial.println(rtcData.counter, DEC);

  // Increment
  rtcData.counter++;
  // Update CRC32 of data
  rtcData.crc32 = calculateCRC32(((uint8_t*) &rtcData) + 4, sizeof(rtcData) - 4);

  // Write struct to RTC memory
  ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData));

  delay(5000);
  ESP.deepSleep(0);
}

void loop() {
}

uint32_t calculateCRC32(const uint8_t *data, size_t length) {
  uint32_t crc = 0xffffffff;
  while (length--) {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) {
        bit = !bit;
      }
      crc <<= 1;
      if (bit) {
        crc ^= 0x04c11db7;
      }
    }
  }
  return crc;
}

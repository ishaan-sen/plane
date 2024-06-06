// rf69 demo tx rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing
// or reliability, so you should only use RH_RF69 if you do not need the
// higher level messaging abilities.
// It is designed to work with the other example RadioHead69_RawDemo_RX.
// Demonstrates the use of AES encryption, setting the frequency and
// modem configuration.

#include <RH_RF69.h>
#include <SPI.h>

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0

#define RFM69_CS 8
#define RFM69_INT 3
#define RFM69_RST 4
#define LED 13

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0; // packet counter, we increment per xmission

void setup() {
  Serial.begin(115200);
  // while (!Serial) delay(1); // Wait for Serial Console (comment out line if
  // no computer)

  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 TX Test!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1) {
    }
  }
  Serial.println("RFM69 radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for
  // low power module) No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power
  // with the ishighpowermodule flag set like this:
  rf69.setTxPower(
      20, true); // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");
  Serial.print((int)RF69_FREQ);
  Serial.println(" MHz");
}

void loop() {
  delay(50);
  
  uint8_t lstick_y = clamp8(map(analogRead(A0), 180, 780, 0, 255));
  uint8_t lstick_x = clamp8(map(analogRead(A2), 220, 820, 0, 255));

  uint8_t rstick_y = clamp8(map(analogRead(A1), 240, 770, 0, 255));
  uint8_t rstick_x = clamp8(map(analogRead(A3), 70, 850, 0, 255));

  uint8_t lpot = clamp8(map(analogRead(A5), 0, 1024, -30, 255));
  uint8_t rpot = clamp8(map(analogRead(A4), 0, 1024, -30, 255));

  uint8_t lswitch = digitalRead(11);
  uint8_t rswitch = digitalRead(12);

  char radiopacket[] = {lstick_x, lstick_y, rstick_x, rstick_y, lpot, rpot, lswitch, rswitch};
  
  Serial.println("Sending ");
  //Serial.println(radiopacket);

  // Send a message!
  rf69.send((uint8_t *)radiopacket, 8);
  rf69.waitPacketSent();

  Serial.print(lstick_y);
  Serial.print("\t");
  Serial.print(lstick_x);
  Serial.print("\t");
  Serial.print(rstick_y);
  Serial.print("\t");
  Serial.print(rstick_x);
  Serial.print("\t");
  Serial.print(lpot);
  Serial.print("\t");
  Serial.print(rpot);
  Serial.print("\t");
  Serial.print(lswitch);
  Serial.print("\t");
  Serial.print(rswitch);
  Serial.println(" ");
}

uint8_t clamp8(int val){
  if(val > 255){
    val = 255;
  }
  if(val < 0){
    val = 0;
  }
  uint8_t out = (uint8_t) val;
  return out;
}

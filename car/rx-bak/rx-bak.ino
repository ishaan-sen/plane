#include <RH_RF69.h>
#include <SPI.h>
#include <Servo.h>

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0

#define RFM69_CS 8
#define RFM69_INT 3
#define RFM69_RST 4
#define LED 13

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

Servo esc;
Servo steer;

uint8_t recv_buf[RH_RF69_MAX_MESSAGE_LEN];
uint8_t recv_len = 0;

uint8_t *lstick_x = &(recv_buf[0]);
uint8_t *lstick_y = &(recv_buf[1]);
uint8_t *rstick_x = &(recv_buf[2]);
uint8_t *rstick_y = &(recv_buf[3]);
uint8_t *lpot = &(recv_buf[4]);
uint8_t *rpot = &(recv_buf[5]);
uint8_t *lswitch = &(recv_buf[6]);
uint8_t *rswitch = &(recv_buf[7]);

unsigned long current_millis = 0;
unsigned long last_comms_millis = 0;

void setup() {
  
  recv_buf[2] = 153;

  esc.attach(5);
  esc.writeMicroseconds(950);

  steer.attach(12);
  steer.writeMicroseconds(1500);

  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1)
      ;
  }

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for
  // low power module) No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power
  // range from 14-20 for power, 2nd arg must be true for 69HCW
  rf69.setTxPower(20, true);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
}

void loop() {
  current_millis = millis();

  if (rf69.available()) {
    if (rf69.recv(recv_buf, &recv_len)) {
      if (recv_len) {
        last_comms_millis = current_millis;
        Serial.println("received signal");
      }
    }
  }

  if (recv_buf[3] > 128) {
    digitalWrite(13, 1);
  } else {
    digitalWrite(13, 0);
  }

  if (current_millis - last_comms_millis > 250) {
    esc.writeMicroseconds(map(recv_buf[3], 0, 255, 1000, 1500));
  } else {
    esc.writeMicroseconds(950);
  }

  steer.writeMicroseconds(map(recv_buf[2], 255, 0, 1400, 1800));
}

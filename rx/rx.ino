#include <Adafruit_LSM6DS33.h>
#include <ArduPID.h>
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

RH_RF69 rf69(RFM69_CS, RFM69_INT);
Adafruit_LSM6DS33 lsm6ds33;

Servo esc;
Servo elevator;
Servo rudder;
Servo ailerons;

uint8_t lx;
uint8_t ly;
uint8_t lt;
uint8_t rx;
uint8_t ry;
uint8_t rt;
uint8_t lb;
uint8_t rb;

ArduPID controller;

double setpoint = 0;
double output;


void setup() {
  esc.attach(5);
  esc.writeMicroseconds(950);

  ailerons.attach(9);
  rudder.attach(10);
  elevator.attach(11);

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

  bool status = false;

  status |= lsm6ds33.begin_I2C();

  status |= rf69.init();

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for
  // low power module) No encryption
  status |= rf69.setFrequency(RF69_FREQ);

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power
  // with the ishighpowermodule flag set like this.
  // range from 14-20 for power, 2nd arg must be true for 69HCW

  rf69.setTxPower(20, true);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  if (status) {
    while (1) {
      Serial.print("Init failed: ");
      Serial.println(status);
      delay(50);
    }
  }
}

void loop() {
  delay(20);
  if (rf69.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len)
        return;
      analogWrite(13, (int) buf[2] * 4);
      esc.writeMicroseconds(smap(buf[2]));
      elevator.writeMicroseconds(smap(buf[4]));
      // esc.writeMicroseconds(smap(buf[2]))
      // esc.writeMicroseconds(smap(buf[2]))
      // esc.writeMicroseconds(smap(buf[2]))
      // esc.writeMicroseconds(smap(buf[2]))
      // esc.writeMicroseconds(smap(buf[2]))
      // esc.writeMicroseconds(smap(buf[2]))

    }
  }
}

int smap(uint8_t control){
  return map(control, 0, 254, 1000, 2000);
}

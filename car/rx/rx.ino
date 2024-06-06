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
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

uint8_t lstick_x;
uint8_t lstick_y;
uint8_t rstick_x;
uint8_t rstick_y;
uint8_t lpot;
uint8_t rpot;
uint8_t lswitch;
uint8_t rswitch;

ArduPID controller;

double setpoint = 0;
double output;

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

unsigned long current_millis = 0;
unsigned long last_millis = 0;
unsigned long last_comms_millis = 0;

double theta = 0;

void setup() {
  controller.begin(&theta, &output, &setpoint, 1, 0.1, 0);
  controller.setWindUpLimits(-3, 3);
  controller.setOutputLimits(-1, 1);
  controller.reverse();

  esc.attach(5);
  esc.writeMicroseconds(950);

  servo2.attach(9);
  servo3.attach(10);
  servo4.attach(11);
  servo5.attach(12);

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

  rf69.setTxPower( 20, true); 

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  if (status) {
    while(1){
      Serial.print("Init failed: ");
      Serial.println(status);
      delay(50);
    }
  }

  // clang-format off
  lstick_x = 128;
  lstick_y = 128;
  rstick_x = 128;
  rstick_y = 128;
  lpot     = 128;
  rpot     = 128;
  lswitch  = 1;
  rswitch  = 0; // start out with stabilization turned on
  // clang-format on
}

void loop() {
  delay(20);
  current_millis = millis();

  lsm6ds33.getEvent(&accel, &gyro, &temp);

  theta += (gyro.gyro.z + 0.013) * (double) (current_millis - last_millis) * 0.001;
  last_millis = current_millis;
  
  controller.compute();
  if (rf69.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len)
        return;

      lstick_x = buf[0];
      lstick_y = buf[1];
      rstick_x = buf[2];
      rstick_y = buf[3];
      lpot     = buf[4];
      rpot     = buf[5];
      lswitch  = buf[6];
      rswitch  = buf[7];
    }
    last_comms_millis = current_millis;
  }


  if (lswitch) {
    servo5.writeMicroseconds(constrain(map(output, -1, 1, 1400, 1800), 1400, 1800));
  } else {
    servo5.writeMicroseconds(map(rstick_x, 255, 0, 1300, 1900));
  }

  if (current_millis - last_comms_millis > 500) {
    esc.writeMicroseconds(950);
  } else {
    esc.writeMicroseconds(map(rstick_y, 5, 255, 1000, 2000));
  }
}

#include <RH_RF69.h>

/* Pinouts
 * Radio VIN  | 5V 
 * Radio GND  | MINI GND | GND
 * Radio SCK  | MINI 13
 * Radio MISO | MINI 12
 * Radio MOSI | MINI 11
 * Radio CS   | MINI 4
 * Radio RST  | MINI 2
 * Radio G0   | MINI 3
 */

#define RFM69r_CS 4
#define RFM69r_RST 2
#define RFM69r_INT 3

#define DEBUG 1		// turn to 0 when using the cansat.exe
#define FREQ 434  // must match sender

RH_RF69 rf69r(RFM69r_CS, RFM69r_INT);

void setup() {
  Serial.begin(9600);
  radio_init_receiver();
}

void loop() {
  receive_data();
}

void print(String msg) {
  if(DEBUG) Serial.println(msg);
}

void radio_init_receiver(){
  pinMode(RFM69r_RST, OUTPUT);
  digitalWrite(RFM69r_RST, LOW);

  print("Cansat Receiver!");

  // manual reset
  digitalWrite(RFM69r_RST, HIGH);
  delay(10);
  digitalWrite(RFM69r_RST, LOW);
  delay(10);
  
  if (!rf69r.init()) {
    print("Initialization Failed.");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  
  if (!rf69r.setFrequency(FREQ)) {
    print("Set Frequency Failed.");
  }

  rf69r.setTxPower(20, true);  

  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69r.setEncryptionKey(key);
}

void receive_data(){
  if (rf69r.available()){
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69r.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      Serial.println((char*)buf);
    } else {
      print("Receive failed");
    }
  }
}

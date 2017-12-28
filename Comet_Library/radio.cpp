#include "DeLata.h"

#define RFM69s_CS 9
#define RFM69s_RST 10
#define RFM69s_INT 11

#define RFM69r_CS 4
#define RFM69r_RST 2
#define RFM69r_INT 3

#define packet_size 55

RH_RF69 rf69s(RFM69s_CS, RFM69s_INT);
RH_RF69 rf69r(RFM69r_CS, RFM69r_INT);

namespace dlt 
{

void radio_init(float frequency){
  pinMode(RFM69s_RST, OUTPUT);
  digitalWrite(RFM69s_RST, LOW);
 
  // manual reset
  digitalWrite(RFM69s_RST, HIGH);
  delay(10);
  digitalWrite(RFM69s_RST, LOW);
  delay(10);
 
  while (!rf69s.init()) {
    print(radio_init_failed);
    delay(1000);
  }
  
  print(radio_init_ok);
 
  while (!rf69s.setFrequency(frequency)) {
    print(freq_failed);
    delay(1000);
  }

  print(freq_ok);
  rf69s.setTxPower(20, true); 

  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69s.setEncryptionKey(key);

  return;
}

void radio_init_receiver(float frequency){
  pinMode(RFM69r_RST, OUTPUT);
  digitalWrite(RFM69r_RST, LOW);

  Serial.println("Cansat Receiver!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69r_RST, HIGH);
  delay(10);
  digitalWrite(RFM69r_RST, LOW);
  delay(10);
  
  if (!rf69r.init()) {
    print(radio_init_failed);
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  
  if (!rf69r.setFrequency(frequency)) {
    print(freq_failed);
  }

  rf69r.setTxPower(20, true);  

  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69r.setEncryptionKey(key);

}

void send_data(char *data){

  size_t len = strlen(data);
  size_t i = 0;

  char temp[56];

  for (i = 0; i < len/packet_size; ++i){
    memcpy(temp, data + (i*packet_size), packet_size);
    rf69s.send((uint8_t *)temp, strlen(temp));
    delay(2);
    //print("Waiting for packet to complete...");
    rf69s.waitPacketSent();
    delay(2);
  }

  if (len % packet_size) {
    rf69s.send((uint8_t *)(data + (len - len % packet_size)), strlen((data + (len - len % packet_size))));
    delay(2);
    //print("Waiting for packet to complete...");
    rf69s.waitPacketSent();
    delay(2);
  }
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

}
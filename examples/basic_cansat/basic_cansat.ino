#include <DeLata.h>

/* Pinouts
 * Camera 5V  | Radio VIN | GPS VIN | 5V 
 * Camera GND | Radio GND | GPS GND | M0 GND 
 * Camera RX  | M0 TX
 * Camera TX  | M0 RX
 * Radio G0   | M0 15 (A1)
 * Radio SCK  | M0 SCK
 * Radio MISO | M0 MISO
 * Radio MOSI | M0 MOSI
 * Radio CS   | M0 Pin 6
 * Radio RST  | M0 Pin 5
 * GPS TX     | M0 Pin 10
 * GPS RX     | M0 Pin 11
 */

#define DEBUG 1     // turn to 0 when Serial Monitor is not connected
#define TID 1         
#define FREQ 434	// must match receiver

uint8_t dummy1 = 100;
uint16_t dummy2 = 200;
uint16_t dummy3 = 400;
  
char user_data[32]; // string formated data (must be null terminated)
int user_task(void)
{
  // do user task here

  sprintf(user_data, "Q%d,R%d,S%d", dummy1++, dummy2++, dummy3++);
  return 1; 
}

void setup() {
  dlt::begin(DEBUG, TID, FREQ);
  dlt::setSensorTask(user_task, user_data);
}

void loop() {
  dlt::execute_task();
}


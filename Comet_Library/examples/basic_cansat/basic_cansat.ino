#include <Comet_Library.h>

/* Pinouts
 * Camera 5V  | 5V 
 * Camera GND | Comet GND 
 * Camera RX  | Comet TX
 * Camera TX  | Comet RX
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


#include "DeLata.h"

#define chipSelect 4

namespace dlt
{

void sd_init(){
  while (!SD.begin(chipSelect)) {
    print(sd_init_failed);
    delay(1000);
  } 
}

// "000" to "999"
static void set_filenumber(char *buff, int number) {
  *buff++ = '0' + number / 100;
  *buff++ = '0' + (number / 10) % 10;
  *buff++ = '0' + number % 10;
}

void next_filename(char *fname) {
  static int file_index = -1;
  // increment file index
  if (file_index++ < 0) { // if first file
    strcpy(fname, "IMG000.JPG");
    while (SD.exists(fname) && (file_index++ < 1000)) {
      set_filenumber(fname+3, file_index);
    }
    return;
  }
  set_filenumber(fname+3, file_index);
}

}
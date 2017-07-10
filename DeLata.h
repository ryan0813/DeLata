
#ifndef __DELATA_H__
#define __DELATA_H__

#include "Arduino.h"

#ifndef ARDUINO_SAMD_VARIANT_COMPLIANCE
#error this library is not compatible with the selected board
#endif

namespace dlt
{

void setTimer(int frequency);

}

#endif // __DELATA_H__

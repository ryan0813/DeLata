
#ifndef __DELATA_H__
#define __DELATA_H__

#include "Arduino.h"
#include <SPI.h>
#include <SD.h>
//#include <Adafruit_GPS.h>
#include "Adafruit_VC0706.h"
#include <RH_RF69.h>

#ifndef ARDUINO_SAMD_VARIANT_COMPLIANCE
#error this library is not compatible with the selected board
#endif

namespace dlt
{

void begin(boolean debug, uint8_t team_id, float frequency);
void begin_rcv(boolean debug, float frequency);
void setSensorTask(int(*sensor_task)(void), const char *output_data);
void startTimer4(int frequency, void (*fun_ptr)(void));
void print(String msg);
void sd_init();
void cam_init();
//void gps_init();
void clear_data_buffer(void);
void next_filename(char *fname);
void check_task(void);
void execute_task();
void set_flag(void);
//void gps_getdata();
void send_data(char *data);
void radio_init(float frequency);
void radio_init_receiver(float frequency);
void receive_data();


const char init_msg[] = "Basic Cansat Setup";
const char sensor_saved_msg[] = "Sensors saved";
const char snap_ok[] = "Capture successful";
const char snap_fail[] = "Capture failed";
const char saving_image[] = "Saving image";
const char small_image[] = "Small frame length";
const char done_reading_cam[] = "Done reading from camera";
const char image_saved[] = "Image saved";
const char cam_not_found[] = "Camera not found";
const char set_size_failed[] = "Failed to set image size";
const char radio_init_failed[] = "Radio init failed";
const char radio_init_ok[] = "Radio init ok";
const char freq_failed[] = "Set frequency failed";
const char freq_ok[] = "Set frequency ok";
const char sd_init_failed[] = "SD Card not found";

}

#endif // __DELATA_H__

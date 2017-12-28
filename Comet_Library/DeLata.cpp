
#include "DeLata.h"

#define data_read_count_MAX   15

Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);
//Adafruit_GPS GPS = Adafruit_GPS();

static char *(*get_data)(void) = NULL;

namespace dlt
{

boolean _debug = 0;
boolean _num_data = 0;
File image_file;
char image_name[13];
uint16_t image_len = 0;
uint8_t bytes_to_read = 0;
uint8_t *cam_buffer = NULL;

File sensors_file;      // sensor data file

uint8_t data_read_count = 0;

//char gps_data[100];

String data_values[data_read_count_MAX];
String *p_data_val = NULL;

static int empty_task(void) { return 0; }

static int (*sensor_func)(void) = empty_task;
static const char *sensor_data = NULL;
static int sensor_func_result = 0;

volatile int task_flag = 0;
volatile uint16_t cnt = 0;

uint8_t _team_id = 0;
uint16_t packet_cnt = 0;

char all_data[512];

enum CAMERA_TASKS {
  CAMERA_NONE = 0,
  CAMERA_FRAME_STOP,   // Take picture
  CAMERA_FILE_NEXT,    // Scan available file
  CAMERA_FILE_OPEN,    // Open blank image file
  CAMERA_FRAME_LENGTH, // Get frame length
  CAMERA_BUFF_READ,    // Read picture buffer
  CAMERA_FILE_WRITE,   // Write buffer to file
  CAMERA_FILE_CLOSE,   // Close image file
  CAMERA_FRAME_RESUME, // Resume camera
} camera_task;

enum MAIN_TASKS {
  TASK_NONE = 0,
  TASK_CAMERA,
  TASK_SENSORS,
} main_task;

enum SENSOR_TASKS {     // Masks
  SENSOR_NONE = 0,
  SENSOR_READ,          // Read sensors and print values
  SENSOR_SAVE,          // Save to SD card
} sensor_task;

void begin(boolean debug, uint8_t team_id, float frequency) {
  _debug = debug;
  _team_id = team_id;

  Serial.begin(115200);
  if (debug) while(!Serial);

  print(init_msg);

  main_task = TASK_NONE;
  camera_task = CAMERA_NONE;
  sensor_task = SENSOR_NONE;

  //gps_init();
  sd_init();
  cam_init();
  radio_init(frequency);

  startTimer4(1, set_flag);

  main_task = TASK_CAMERA;
  camera_task = CAMERA_FRAME_STOP;

  return;
}

void begin_rcv(boolean debug, float frequency){
   _debug = debug;
   Serial.begin(115200);
   while(!Serial);	

   radio_init_receiver(frequency);
}

void set_flag(void){
	gps_getdata();
  if (cnt++ > 2){
    task_flag = 1;
    cnt = 0;	
  }
}

void setSensorTask(int(*sensor_task)(void), const char *output_data) {
  sensor_func = sensor_task;
  sensor_data = output_data;
}

void do_sensor_tasks(void) {
  switch (sensor_task) {
  case SENSOR_READ:
    if (data_read_count < data_read_count_MAX) {

      sensor_func_result = sensor_func();
      
      //sprintf(gps_data, "I%02d:%02d:%02d,J%d,K%0.4f,L%0.4f,M%f,N%f,O%f,P%d", (GPS.hour + 8), GPS.minute, GPS.seconds, GPS.fix, GPS.latitudeDegrees, GPS.longitudeDegrees, GPS.speed, GPS.angle, GPS.altitude, GPS.satellites);

      sprintf(all_data, "G%d,H%d,%s,%s,!end", _team_id, packet_cnt++, /*gps_data*/, sensor_data);
      print(all_data);

      // Form data for transmission
      // Required format by the server: 
      // G<TeamID>,H<PacketCnt>,I<GPSTime>,J<Fix>,K<Lat>,L<Lon>,M<Speed>,N<Angle>,O<Alt>,P<Sat>,Q<D1>,R<D2>,S<D3>,...,X<D8>,Y<D9>,Z<D10>

      send_data(all_data);
      if (sensor_func_result > 0) *p_data_val++ = all_data;

      data_read_count++; 
    }
    break;
  case SENSOR_SAVE:

    sensors_file = SD.open("PTD.CSV", FILE_WRITE);
    p_data_val = data_values;

    // Write sensor data to SD Card
    for (int read_count = 0; read_count < data_read_count; read_count++)  {
      sensors_file.println(*p_data_val++);
    }

    image_file.flush();
    sensors_file.close();
    clear_data_buffer();

    print(sensor_saved_msg);

    sensor_task = SENSOR_READ;
    break;
  }
  
  main_task = TASK_CAMERA;
}

void do_camera_tasks(void) {
  switch (camera_task) {
  case CAMERA_FRAME_STOP:
    if (cam.takePicture()) {
      print(snap_ok);
      camera_task = CAMERA_FILE_NEXT;
    } else {
      print(snap_fail);
    }
    break;
  case CAMERA_FILE_NEXT:
    next_filename(image_name);
    camera_task = CAMERA_FILE_OPEN;
    break;
  case CAMERA_FILE_OPEN:
    image_file = SD.open(image_name, FILE_WRITE);
    camera_task = CAMERA_FRAME_LENGTH;
    break;
  case CAMERA_FRAME_LENGTH:
    image_len = cam.frameLength();
    if (image_len>>10) { // 1kB minimum
      print(saving_image);
      camera_task = CAMERA_BUFF_READ;
    } else { // if small frame length
      print(small_image);
      //camera_task = CAMERA_FRAME_STOP; // retake picture
    }
    break;
  case CAMERA_BUFF_READ:
    if (image_len < 1) {
      print(done_reading_cam);
      camera_task = CAMERA_FILE_CLOSE;
      break;
    }
    bytes_to_read = min(64, image_len);
    cam_buffer = cam.readPicture(bytes_to_read);
    if (cam_buffer) {
      image_len -= bytes_to_read;
      camera_task = CAMERA_FILE_WRITE;
    }
    break;
  case CAMERA_FILE_WRITE:
    image_file.write(cam_buffer, bytes_to_read);
    camera_task = CAMERA_BUFF_READ;
    break;
  case CAMERA_FILE_CLOSE:
    image_file.flush();
    image_file.close();
    print(image_saved);
    camera_task = CAMERA_FRAME_RESUME;
    break;
  case CAMERA_FRAME_RESUME:
    if (cam.resumeVideo()) {
      // capture again after saving sensor values
      camera_task = CAMERA_FRAME_STOP;
      main_task = TASK_SENSORS;
      sensor_task = SENSOR_SAVE;
    }
    break;
  }
}

void check_task(void){
  if (sensor_task != SENSOR_READ) {
    sensor_task = SENSOR_READ;
    clear_data_buffer();
  }
  main_task = TASK_SENSORS;
}

void execute_task(){
  gps_getdata();

  if (task_flag){
  	gps_getdata();
  	task_flag = 0;
    check_task();
  }

  switch (main_task) {
  case TASK_CAMERA:
    gps_getdata();
    do_camera_tasks();
    break;
  case TASK_SENSORS:
    gps_getdata();
    do_sensor_tasks();
    break;
  }	
}

void print(String msg) {
  if(_debug) Serial.println(msg);
}

void clear_data_buffer(void) {
  memset(data_values, 0, sizeof(data_values));
  p_data_val = data_values;
  data_read_count = 0;
}

void cam_init(){
  Serial1.begin(9600);

  while (!cam.begin()) {
   	print(cam_not_found);
    delay(1000);
  }

  delay(500);  // required delay for next command
  if (!cam.setImageSize(VC0706_640x480)) {
   	print(set_size_failed);
    return;
  }
}

/*
void gps_init(){
  GPS.begin();
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
}

uint32_t timer = millis();

void gps_getdata(){
  //if (timer > millis())  timer = millis();

  //if (millis() - timer > 1){
  //  timer = millis(); // reset the timer
  
  	GPS.read();

    if (GPS.newNMEAreceived()) {
      if (!GPS.parse(GPS.lastNMEA())) return;
    }
  //}
}*/

}

// G<TeamID>,H<PacketCnt>,I<GPSTime>,J<Fix>,K<Lat>,L<Lon>,M<Speed>,N<Angle>,O<Alt>,P<Sat>,Q<D1>,R<D2>,S<D3>,...,X<D8>,Y<D9>,Z<D10>
#define TID 15
#define packet_size 63

char message[120];
int packet = 1;
int data1 = 10;
int data2 = 20;
int data3 = 30;
int data4 = 40;
int data5 = 50;
int data6 = 60;
int data7 = 70;
int data8 = 80;
int data9 = 90;
int data10 = 100;
int data11 = 110;
int data12 = 120;
int data13 = 130;
int data14 = 140;
int data15 = 150;
int data16 = 160;
int data17 = 170;
int data18 = 180;

void send_data(char *data){

  size_t len = strlen(data);
  size_t i = 0;

  char temp[64];
  for (i = 0; i < len/packet_size; ++i){
    memcpy(temp, data + (i*packet_size), packet_size);
    Serial.println(temp);
  }

  if (len % packet_size) {
    int rem_len = len % packet_size;
    Serial.println((data + (len - rem_len)));
  }
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  sprintf(message, "G%d,H%d,I%d,J%d,K%d,L%d,M%d,N%d,O%d,P%d,Q%d,R%d,S%d,T%d,U%d,V%d,W%d,X%d,Y%d,Z%d,!!!", TID, packet++, data1++, data2++, data3++, data4++, data5++, data6++, data7++, data8++, data9++, data10++, data11++, data12++, data13++, data14++, data15++, data16++, data17++, data18++);
  send_data(message);
  delay(1000);
}

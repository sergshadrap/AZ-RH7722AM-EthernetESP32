#include "config.h"
#include <Arduino.h>
#include "Mpp32Devices.h"
#include "Mpp32Server.h"

#define BAUD2   9600 //Second serial baud rate
#define RX2     5 // Second hardware serial 
#define TX2     17 

const char* DeviceVersion = "RH7722Mpp32Eth 1.1.0"; // Handled Events in MppServer 
static const char *P_PERIOD = "Period"; 
static const char* Properties[] = { P_PERIOD, //
        NULL };


extern bool eth_connected;

HardwareSerial MSerial2(1);

class MppServer mppserver(DeviceVersion, Properties);

class MppDevice temp; //Temperature
class MppDevice hum; // Humidity
class MppDevice co2; // CO2 
class MppDevice dw; // dew point
class MppDevice wb; // wet bulb


unsigned int CO2=0; 
float Humidity=0; 
float Temperature=0; 
float DewPoint=0; 
float WetBulb=0; 
 unsigned int oldCO2; 
float oldHumidity; 
float oldTemperature; 
float oldDewPoint; 
float oldWetBulb; 


void onReceiveFn() {
const int bufferSize = 60; 
char buffer[bufferSize]; 
int bufferIndex = 0;


  const char* header = "$CO2:Air:RH:DP:WBT"; 
  const int headerLength = strlen(header)+2; //+1 byte of LRC +1 byte of end of terminating zero
  
    while (MSerial2.available()) {
       char inChar = (char)MSerial2.read();
    //    Serial.printf("%02X ",inChar);
       if ((bufferIndex < bufferSize - 1)) {
            if(inChar!='\r' && inChar!='\n') 
            {
              if(bufferIndex>=headerLength) // filtering out the header by filling '0'
              buffer[bufferIndex] = inChar; 
              else buffer[bufferIndex] = '0';
              bufferIndex++;
            } 
        } else {      // buffer overflow control
          break;
        }
    }
   buffer[bufferIndex] ='\0';

    sscanf(buffer, "00000000000000000000C%dppm:T%fC:H%f%%:d%fC:w%fC", &CO2, &Temperature, &Humidity, &DewPoint, &WetBulb);
// Serial.printf("Received data: %s headlen:%d buf ind:%d i:%d\n",buffer,headerLength,bufferIndex,i);
}


  
//The setup function is called once at startup of the sketch
void setup() {
	Serial.begin(115200); 
  MSerial2.begin(BAUD2,SERIAL_8N1,RX2,TX2);
	Serial.println("ready...");
// MSerial2.setRxTimeout(200);
 MSerial2.onReceive(onReceiveFn,true);  // sets a RX callback function for Serial2

mppserver.manageDevice(&temp, getDefaultUDN(MppAnalog) + "_T");
 mppserver.manageDevice(&hum, getDefaultUDN(MppAnalog) + "_H");
  mppserver.manageDevice(&co2, getDefaultUDN(MppAnalog) + "_CO");
   mppserver.manageDevice(&dw, getDefaultUDN(MppAnalog) + "_DW");
    mppserver.manageDevice(&wb, getDefaultUDN(MppAnalog) + "_WB");
    
 mppserver.begin();
}

#define checkin 10000
unsigned long next = millis();

// The loop function is called in an endless loop
void loop() {
	unsigned long now = millis();


 mppserver.handleClients();
 mppserver.handleCommand();


	if (now > next && eth_connected) {
  // Serial.printf("Received data: %s \n",buf.c_str());
  //          bufferIndex = 0;  
		Serial.printf("heap=%lus at %lus\n", ESP.getFreeHeap(), now / 1000);

		next = now + checkin;
if(CO2!=oldCO2) {
  co2.put(VALUE, String(CO2));
  co2.put(STATE, CO2 == 0 ? "off" : "on");
  oldCO2=CO2;
  co2.notifySubscribers();
}

if(Temperature!=oldTemperature) {
  temp.put(VALUE, String(Temperature));
  temp.put(STATE, Temperature == 0 ? "off" : "on");
  oldTemperature=Temperature;
  temp.notifySubscribers();
}

if(Humidity!=oldHumidity) {
  hum.put(VALUE, String(Humidity));
  hum.put(STATE, Humidity == 0 ? "off" : "on");
  oldHumidity=Humidity;
  hum.notifySubscribers();
}

if(DewPoint!=oldDewPoint) {
  dw.put(VALUE, String(DewPoint));
  dw.put(STATE, DewPoint == 0 ? "off" : "on");
  oldDewPoint=DewPoint;
  dw.notifySubscribers();
}

if(WetBulb!=oldWetBulb) {
  wb.put(VALUE, String(WetBulb));
  wb.put(STATE, WetBulb == 0 ? "off" : "on");
  oldWetBulb=WetBulb;
  wb.notifySubscribers();
}

	}
}

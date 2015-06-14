#include <OneWire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PID_v1.h>
#define RelayPin 6

int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
double Setpoint;
double Input, Output;

int sensorPin = A5; // select the input pin for the potentiometer
int sensorValue = 0; // variable to store the value coming from the sensor
//Specify the links and initial tuning parameters

float Kp = 200;
float Ki = 50; 
float Kd = 0;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

int WindowSize = 2000;
unsigned long windowStartTime;

const int numReadings = 30;

double readings[numReadings]; // the readings from the analog input
int index = 0; // the index of the current reading
double total = 0; // the running total
double input = 0; // the average

int inputPin = A0;

//Temperature chip i/o
OneWire ds(DS18S20_Pin); // on digital pin 10
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x90, 0xa4, 0xdr, 0x00, 0xa5, 0x59 }; //I made one up here it should be listed on your ethernet shield box
IPAddress server(184, 188, 16, 158); // place your server IP address here, I made on up

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup(void) {


pinMode(6, OUTPUT);
windowStartTime = millis();
Serial.begin(9600);
Setpoint = 71;
//tell the PID to range between 0 and the full window size
myPID.SetOutputLimits(0, WindowSize);

//turn the PID on
myPID.SetMode(AUTOMATIC);

for (int thisReading = 0; thisReading < numReadings; thisReading++)
readings[thisReading] = 0; 

analogReference(EXTERNAL);
}

void loop(void) {
analogReference(DEFAULT);
sensorValue = analogRead(sensorPin)/7.42+75;
Setpoint = sensorValue;
analogReference(EXTERNAL);
total= total - readings[index]; 
float temperature = getTemp()*9/5+32;
readings[index] = temperature;
total= total + readings[index]; 

index = index + 1; 

// if we're at the end of the array...
if (index >= numReadings) 
// ...wrap around to the beginning:
index = 0;

// calculate the average:
Input = total / numReadings; 
// send it to the computer as ASCII digits

myPID.Compute();


Serial.print(Input*1);
Serial.print(" ");
Serial.print(Output);
Serial.print(" ");
Serial.print(Setpoint);
Serial.print(" ");
Serial.print(index);
Serial.print(" ");
Serial.print(numReadings);
Serial.print(" ");
Serial.print(total);
Serial.print(" ");
Serial.print(readings[index]);
Serial.print(" ");

Serial.println(temperature);

 

/************************************************
* turn the output pin on/off based on pid output
************************************************/
if(millis() - windowStartTime>WindowSize)
{ //time to shift the Relay Window
windowStartTime += WindowSize;
}
if(Output < millis() - windowStartTime) digitalWrite(RelayPin,LOW);
else digitalWrite(RelayPin,HIGH);

if (index == 29) 
{

(Ethernet.begin(mac)); 
// give the Ethernet shield a second to initialize:
delay(1000); 
Serial.println("connecting...");

// if you get a connection, report back via serial:
(client.connect(server, 80));
Serial.println("connected");
// Make a HTTP request:
client.print("GET http://example.com/data.php?Temperature="); //place your server address here
client.print(temperature);
client.print("&Setpoint=");
client.print(Setpoint);
client.print("&Input=");
client.print(Input);
client.print("&Kp=");
client.print(Kp);
client.print("&Ki=");
client.print(Ki);
client.print("&Kd=");
client.print(Kd);
client.print("&Heater=");
client.print(Output/WindowSize*100);
client.println(" HTTP/1.0");
client.println("Host: http://example.com");
client.println();
client.stop();
}
}
float getTemp(){
//returns the temperature from one DS18S20 in DEG Celsius

byte data[12];
byte addr[8];

if ( !ds.search(addr)) {
//no more sensors on chain, reset search
ds.reset_search();
return -1000;
}

if ( OneWire::crc8( addr, 7) != addr[7]) {
Serial.println("CRC is not valid!");
return -1000;
}

if ( addr[0] != 0x10 && addr[0] != 0x28) {
Serial.print("Device is not recognized");
return -1000;
}

ds.reset();
ds.select(addr);
ds.write(0x44,1); // start conversion, with parasite power on at the end

delay(1000);

byte present = ds.reset();
ds.select(addr);
ds.write(0xBE); // Read Scratchpad
for (int i = 0; i < 9; i++) { // we need 9 bytes
data[i] = ds.read();
}

ds.reset_search();

byte MSB = data[1];
byte LSB = data[0];

float tempRead = ((MSB << 8) | LSB); //using two's compliment
float TemperatureSum = tempRead / 16;

return TemperatureSum;

}

- See more at: http://fermentationriot.com/arduinopid.php#sthash.PNFZ8nMW.dpuf

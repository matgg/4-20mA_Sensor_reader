
 /*
 *  4 - 20 mA sensor
 *
 *  - Server the output of a 4-20mA sensor as a web page.
 *  - Connect whit Blynk services for control with smartphone.
 *  - Read & write in a SD Memory
 *
 *  Use an Arduino Mega and Ethernet shield.
 *  Sensor1 input: pin A8
 *
 *  created 16 April 2016
 *  by Matias Gonzalez
 *
 */

#define BLYNK_PRINT Serial
#include <Ethernet.h>
#include <SPI.h>
#include <BlynkSimpleEthernet.h>
#include <SD.h>

//You should get Auth Token in the blynk App
//Go to the project Settings

char auth[] = "9ffdbbb50df5437aa5684bcf3970811a";


//assign a MAC address for the Ethernet controller.
//fill in your address here:
//IPAddress server_ip (10, 0, 0, 10);

// Mac address should be different for each device in your LAN
byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress arduino_ip ( 192,   168,   1,  20);
IPAddress dns_ip     (  8,   8,   8,   8);
IPAddress gateway_ip ( 192,   168,   1,   1);
IPAddress subnet_mask(255, 255, 255,   0);

// Initialize the Ethernet sercer library with the IP address an port yoiu want to use
// (port 80 is default fot HTTP):
EthernetServer server(80);

//pins used for the connection with the sensors
const int sensor1Pin = A8;
const int chipSelect = 4; //chipselect for SDCard
const int SSPin = 53; //SS pin for Mega 2560
// Keep this flag not to re-sync on every reconnection
bool isFirstConnect = true;

int sensor1Value = 0;
float pressure = 0;
long lastReadingTiem = 0;
char file[] = "values.htm";


void setup() {

  //start the Ethernet connection and the server:
  Ethernet.begin(arduino_mac, arduino_ip);
  server.begin();

  Serial.begin(9600);
  //Blynk.begin(auth, server_ip, 8080, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
  Blynk.begin(auth, "cloud.blynk.cc", 8442, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);

  //Initialize the data ready pin
  pinMode(sensor1Pin, INPUT);
  //Initialize the SD card
  Serial.print("Probando memoria SD...");
  pinMode(SSPin, OUTPUT);         // Don't delete this line, is for SD
  if (!SD.begin(chipSelect))
    {  Serial.println("No se encuentra SD");
        return;    // NO sigas
    }
  Serial.println("SD encontrada.");


  //Give the ethernet shield time to set up:
  delay(1000);


}



void loop() {

  Blynk.run();

  //Read the values form sensor
  getData();
  //Write value in SDCard
  writeValueInSD(file, sensor1Value);
  //listen for incoming Ethernet connections:
  listenForEthernetClients();


  delay(1000);

}

getData(){
  Serial.println("Obteniendo valor:");

  // read the value from the sensor:
  sensor1Value = analogRead(sensor1Pin);
  Serial.print("Valor leido: ");
  Serial.println(sensor1Value);

}

void listenForEthernetClients(){
  //listen for incoming clients
  EthernetClient client = server.available();
  if(client){
    Serial.println("Cliente");
    //an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //if you've gotten to the end of the line (recieved a newline
        //character) and the line is blank, the http request has ended,
        //so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          //send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          //print the current read, in HTML format:
          client.print("Valor leido: ");
          client.print(sensor1Value);
          client.print("<br />");
          break;
        }
        if (c == '\n'){
          //you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          //you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    //give the web browser time to receive the data
    delay(1);
    //close the connection:
    client.stop();
  }
}

// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
  if (isFirstConnect) {
    // Request Blynk server to re-send latest values for all pins
    Blynk.syncAll();

    // You can also update an individual Virtual pin like this:
    //Blynk.syncVirtual(V0);

    isFirstConnect = false;
  }
}

void writeValueInSD(char file[], int value){
  File dataFile = SD.open(file, FILE_WRITE);
  if (dataFile) {
    dataFile.println(value);
  } else {
    /* code */
  }
}
void readSDCard(char file){


}

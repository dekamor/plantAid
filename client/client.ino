#include <SPI.h>
#include <WiFiNINA.h>
#include <dht.h>
#define dht_apin A1 // Analog Pin sensor is connected to
 
dht DHT;

// Configure the pins used for the ESP32 connection
#if defined(ADAFRUIT_FEATHER_M4_EXPRESS) || \
  defined(ADAFRUIT_FEATHER_M0_EXPRESS) || \
  defined(ADAFRUIT_FEATHER_M0) || \
  defined(ARDUINO_AVR_FEATHER32U4) || \
  defined(ARDUINO_NRF52840_FEATHER) || \
  defined(ADAFRUIT_ITSYBITSY_M0) || \
  defined(ADAFRUIT_ITSYBITSY_M4_EXPRESS) || \
  defined(ARDUINO_AVR_ITSYBITSY32U4_3V) || \
  defined(ARDUINO_NRF52_ITSYBITSY)
  // Configure the pins used for the ESP32 connection
  #define SPIWIFI       SPI  // The SPI port
  #define SPIWIFI_SS    13   // Chip select pin
  #define ESP32_RESETN  12   // Reset pin
  #define SPIWIFI_ACK   11   // a.k.a BUSY or READY pin
  #define ESP32_GPIO0   -1
#elif defined(ARDUINO_AVR_FEATHER328P)
  #define SPIWIFI       SPI  // The SPI port
  #define SPIWIFI_SS     4   // Chip select pin
  #define ESP32_RESETN   3   // Reset pin
  #define SPIWIFI_ACK    2   // a.k.a BUSY or READY pin
  #define ESP32_GPIO0   -1
#elif defined(TEENSYDUINO)
  #define SPIWIFI       SPI  // The SPI port
  #define SPIWIFI_SS     5   // Chip select pin
  #define ESP32_RESETN   6   // Reset pin
  #define SPIWIFI_ACK    9   // a.k.a BUSY or READY pin
  #define ESP32_GPIO0   -1
#elif defined(ARDUINO_NRF52832_FEATHER )
  #define SPIWIFI       SPI  // The SPI port
  #define SPIWIFI_SS    16   // Chip select pin
  #define ESP32_RESETN  15   // Reset pin
  #define SPIWIFI_ACK    7   // a.k.a BUSY or READY pin
  #define ESP32_GPIO0   -1
#elif !defined(SPIWIFI_SS)   // if the wifi definition isnt in the board variant
  // Don't change the names of these #define's! they match the variant ones
  #define SPIWIFI       SPI
  #define SPIWIFI_SS    10   // Chip select pin
  #define SPIWIFI_ACK    7   // a.k.a BUSY or READY pin
  #define ESP32_RESETN   5   // Reset pin
  #define ESP32_GPIO0   -1   // Not connected
#endif


char ssid[] = "VIRGIN787";        // your network SSID (name)
char pass[] = "2021please";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

// char server[] = "wifitest.adafruit.com";    // name address for adafruit test
byte server[] = { 192, 168, 2, 13 }; // "127.0.0.1"
char path[]   = "/testwifi/index.html";


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 100L * 50L; // delay between updates, in milliseconds


int soilReading = 0; //value for storing moisture value 
int soilPin = A0;//Declare a variable for the soil moisture sensor 
int soilPower = 7;//Variable for Soil moisture Power

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);
  while (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    delay(1000);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
  Serial.print("Found firmware "); Serial.println(fv);

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  do {
    status = WiFi.begin(ssid, pass);
    delay(100);     // wait until connection is ready!
  } while (status != WL_CONNECTED);

  Serial.println("Connected to wifi");
  printWifiStatus();

  pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
  digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor

/*

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 65432)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    //client.print("GET "); client.print(path); client.println(" HTTP/1.1");
    // client.print("Host: "); client.println(String(server));
    client.write("1234"); //demo of write of actual reading value
    //client.println("Connection: close");
    //client.println();
  }
 */
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
 /*
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    // do nothing forevermore:
    while (true);
  }
*/
  if (millis() - lastConnectionTime > postingInterval) {
    sendReadings();
  }
  
}

void sendReadings(){
  //client.stop();

  if(client.connect(server, 65432)){
    Serial.println("connected to server");
    
    Serial.println(readHumidity());
    String hum_str = String(readHumidity());
    int str_len = hum_str.length()+1;
    char hum_char[str_len];
    hum_str.toCharArray(hum_char, str_len);

    
    Serial.println(readTemp());
    String temp_str = String(readTemp());
    str_len = temp_str.length()+1;
    char temp_char[str_len];
    temp_str.toCharArray(temp_char, str_len);

    
    
    // Make a HTTP request:
    //client.print("GET "); client.print(path); client.println(" HTTP/1.1");
    // client.print("Host: "); client.println(String(server));
    String soil_str = String(readSoil());
    str_len = soil_str.length() + 1; 
    char soil_char[str_len];
    soil_str.toCharArray(soil_char, str_len);

    char msg[40];
    char *comma =  ",";
    strcpy(msg, hum_char);
    strcat(msg, comma);
    strcat(msg, temp_char);
    strcat(msg, comma);
    strcat(msg, soil_char);
    Serial.println(msg);
    
    client.write(msg); //demo of write of actual reading value

    //client.println("Connection: close");
    //client.println();
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
  
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

int readSoil()
{

    digitalWrite(soilPower, HIGH);//turn D7 "On"
    delay(10);//wait 10 milliseconds 
    soilReading = analogRead(soilPin);//Read the SIG value form sensor 
    Serial.print("Soil moisture reading: ");
    Serial.println(soilReading);
    digitalWrite(soilPower, LOW);//turn D7 "Off"
    return soilReading;//send current moisture value
}

int readTemp()
{
  DHT.read11(dht_apin);
  return DHT.temperature;
}

int readHumidity()
{
  DHT.read11(dht_apin);
  return DHT.humidity;
}

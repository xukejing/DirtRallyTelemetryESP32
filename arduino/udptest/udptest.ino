// Show leds based on car RPMs
#include <FastLED.h>
#define NUM_LEDS 53
#define DATA_PIN 27
CRGB leds[NUM_LEDS];
int hsvH[NUM_LEDS];
int hsvV[NUM_LEDS];
#include <WiFi.h>
#include <NetworkUdp.h>

// WiFi network name and password:
const char *networkName = "***";
const char *networkPswd = "***";

//Change the information of the computer that sends UDP data
const char *udpAddress = "192.168.1.8";
const int udpPort = 20778;
//Are we currently connected?
boolean connected = false;

//The udp library class
NetworkUDP udp;

int ledPin = 22;
boolean blink1 = false;
void setup() {
  // Initialize hardware serial:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1);
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
  sleep(1);
  if (connected) {
    digitalWrite(ledPin, 0);
  } else
    digitalWrite(ledPin, 1);
  udp.begin(udpPort);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  int i=0;
  //leds1
  int midled=int(NUM_LEDS/2);
  for(i=0;i<midled;i++)
  {
    hsvH[i]=170+85*i/midled;
    leds[i].setHSV(hsvH[i],255,128);
  }
  for(i=midled;i<NUM_LEDS;i++)
  {
    hsvH[i]=255-85*(i-midled)/(NUM_LEDS-midled);
    leds[i].setHSV(hsvH[i],255,128);
  }
  FastLED.show();
}

void loop() {
  readMyUDP();
}

void connectToWiFi(const char *ssid, const char *pwd) {
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);  // Will call WiFiEvent() from another thread.

  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

// WARNING: WiFiEvent is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      //When connected set
      Serial.print("WiFi connected! IP address: ");
      Serial.println(WiFi.localIP());
      //initializes the UDP state
      //This initializes the transfer buffer
      udp.begin(WiFi.localIP(), udpPort);
      connected = true;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      connected = false;
      break;
    default: break;
  }
}

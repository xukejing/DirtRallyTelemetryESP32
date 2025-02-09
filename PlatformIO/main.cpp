#include <Arduino.h>

// Show leds based on car RPMs
#include <FastLED.h>
#define NUM_LEDS 53
#define DATA_PIN 27
#define FAN_PWM_PIN 13
#define PWM_FREQ 25000  // 25kHz 频率
#define PWM_RESOLUTION 8  // 8 位分辨率（0-255）
CRGB leds[NUM_LEDS];
int hsvH[NUM_LEDS];
int hsvV[NUM_LEDS];
#include <WiFi.h>

// WiFi network name and password:
const char *networkName = "xxxx";
const char *networkPswd = "xxxxx";

//IP address to send UDP data to:
// either use the ip address of the server or
// a network broadcast address
const char *udpAddress = "192.168.1.8";
const int udpPort = 20778;

//Are we currently connected?
boolean connected = false;

//The udp library class
#include <WiFiUdp.h>
WiFiUDP udp;

boolean blink1 = false;

union charFloat {
  char str[264];
  float value[66];
};

void connectToWiFi(const char *ssid, const char *pwd) ;
void WiFiEvent(WiFiEvent_t event);
void readMyUDP();
void myleds(float RPM) ;
void fandebug() ;
void myfan(float velocity);


void setup() {
  // Initialize hardware serial:
  Serial.begin(115200);
  ledcSetup(0,PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(FAN_PWM_PIN,0);
  // 设置初始占空比（0-255）
  ledcWrite(0,25);
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
  sleep(1);
  if (connected) {
    sleep(1);
  } else
    sleep(1);
  udp.begin(udpPort);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  int i=0;
  //leds1
  int midled=int(NUM_LEDS/2);
  for(i=0;i<midled;i++)
  {
    hsvH[i]=170+85*i/midled;
    leds[i].setHSV(hsvH[i],255,28);
  }
  for(i=midled;i<NUM_LEDS;i++)
  {
    hsvH[i]=255-85*(i-midled)/(NUM_LEDS-midled);
    leds[i].setHSV(hsvH[i],255,28);
  }
  FastLED.show();

}

void loop() {
  readMyUDP();
  //fandebug();
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

void readMyUDP()
 {
  int packetSize = udp.parsePacket();
  if (packetSize == 264) {
    union charFloat bufFloat;
    udp.read(bufFloat.str, 264);
    float v=bufFloat.value[7] * 3.6;
    myfan(v);
    Serial.print("v=");
    Serial.print(bufFloat.value[7] * 3.6);
    // Serial.print("  ");
    // Serial.print("gear=");
    // Serial.print(bufFloat.value[33]);
    // Serial.print("  ");
    // Serial.print("RPM=");
    // Serial.print(bufFloat.value[37] * 10.0);
    // Serial.println(";");
    float rpmMin=178;
    float rpm2 = (bufFloat.value[37] - rpmMin)/ (bufFloat.value[63]- rpmMin);
    if (bufFloat.value[63]==0)
    rpm2=0;
    myleds(rpm2);
    Serial.print("RPM:");
    Serial.print(bufFloat.value[37]);

    // Serial.print("  ");
    // Serial.print(bufFloat.value[63]);
    Serial.println("");
  }
}


void myleds(float RPM) {
  int midled = int(NUM_LEDS / 2);
  int bright = int(midled * 128 * RPM);
  int i = 0;
  for (i = 0; i < midled; i++) {
    if (bright > (i * 128))
      hsvV[i] = 128;
    else if ((bright > ((i - 1) * 128)))
      hsvV[i] = bright - ((i - 1) * 128);
    else
      hsvV[i] = 0;
  }
  for (i = midled; i < NUM_LEDS; i++) {
    if (bright > ((NUM_LEDS - i - 1) * 128))
      hsvV[i] = 128;
    else if ((bright > ((NUM_LEDS - i - 2) * 128)))
      hsvV[i] = bright - ((NUM_LEDS - i - 2) * 128);
    else
      hsvV[i] = 0;
  }
  if (RPM > 0.95) {
    blink1 = !blink1;
  } else
    blink1 = false;
  if (blink1) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i].setHSV(255, 255, hsvV[i] * 1.5);
    }
  } else {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i].setHSV(hsvH[i], 255, hsvV[i]);
    }
  }

  FastLED.show();
}
void myfan(float velocity) {
  int pwm=25;
  if(velocity>10)
  {
    pwm=int((velocity-10)*1.643+25);
  }
  if (pwm>255)
  pwm=255;
  ledcWrite(0,pwm);
}
void fandebug() {
  int i=0;
  ledcWrite(0,0);
  delay(2000);
  for(i=0;i<255;i=i+5)
  {
    Serial.print("set pwm: ");
    Serial.println(i);
    ledcWrite(0,i);
    delay(2000);
  }
}
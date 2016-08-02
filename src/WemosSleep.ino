#include <ArduinoOTA.h>

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <ESP8266mDNS.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic


#include <ESP8266HTTPClient.h>

#include <Adafruit_NeoPixel.h>

#include <time.h>

#include <WiFiUdp.h>

#define min(a,b) ((a<b)?a:b)

ADC_MODE(ADC_VCC);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, D2, NEO_GRB + NEO_KHZ800);
WiFiUDP udp;
IPAddress ipMulti(236, 42, 42, 42);

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  WiFiManager wifiManager;

  wifiManager.autoConnect();

  pixels.begin();
  ArduinoOTA.begin();

  Serial.println(WiFi.getMode());
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // only required for https (initializes sntp)
  udp.beginMulticast(WiFi.localIP(), ipMulti, 4242);
}

void loop() {
  static int count = 0;

  int numBytes = udp.parsePacket();
  if (numBytes) {
    char buf[5];
    udp.read(buf, min(numBytes, 5)); // TODO: use data for color/pattern

    pixels.setPixelColor(0, pixels.Color(0,0,255));
    pixels.show();
    delay(1000);
    pixels.clear();
    pixels.show();
  }

  if (count % 20 == 0) {
    for (int i = 0; i < 50; i++)
    {
      pixels.setPixelColor(0, pixels.Color(i, i * 5, i));
      pixels.show();
      delay(10);
    }
    for (int i = 50; i >= 0; i--)
    {
      pixels.setPixelColor(0, pixels.Color(i, i * 5, i));
      pixels.show();
      delay(10);
    }
  }
  
  ArduinoOTA.handle();
  if (count % 6 == 17) { // disabled
    uint16_t vcc = ESP.getVcc();
    HTTPClient http;
    int res = http.begin("https://dweet.io/dweet/for/esp85e742?vcc=" + String(vcc/1024.0), "5C D1 3E DC D9 F8 48 97 1C 55 8F 1A DA 31 8F 5B FE 2E 14 B6");
    res = http.GET();
    http.end();
  }
  delay(500);
  count += 1;
}

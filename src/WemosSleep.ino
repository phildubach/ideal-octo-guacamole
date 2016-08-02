#include <ArduinoOTA.h>

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <ESP8266mDNS.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic


#include <ESP8266HTTPClient.h>

#include <time.h>

#include <WiFiUdp.h>

#include <TempSensor.h>

#define min(a,b) ((a<b)?a:b)

WiFiUDP udp;
IPAddress ipMulti(236, 42, 42, 42);

TempSensor temp(2);

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  WiFiManager wifiManager;

  wifiManager.autoConnect();

  ArduinoOTA.begin();

  Serial.println(WiFi.getMode());
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // only required for https (initializes sntp)
  udp.beginMulticast(WiFi.localIP(), ipMulti, 4242);

  Serial.print("Temp sensor detected: ");
  Serial.println(temp.sample());
  Serial.println(ESP.getChipId());
}

void loop() {
  static int count = 0;

  int numBytes = udp.parsePacket();
  if (numBytes) {
    char buf[5];
    udp.read(buf, min(numBytes, 5)); // TODO: use data for color/pattern
    delay(1000);
  }

  ArduinoOTA.handle();
  if (count % 120 == 0) { // disabled
    HTTPClient http;
    String curTemp = String(temp.read());
    Serial.println("Temperature: " + curTemp);
    int res = http.begin("http://dweet.io/dweet/for/esp" + String(ESP.getChipId()) + "?temp=" + curTemp); //, "5C D1 3E DC D9 F8 48 97 1C 55 8F 1A DA 31 8F 5B FE 2E 14 B6");
    res = http.GET();
    http.end();
  }
  delay(500);
  count += 1;
}

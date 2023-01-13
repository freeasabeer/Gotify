#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "Gotify.h"

uint8_t serverfingerprint[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
WiFiClientSecure sclient;
Gotify Gotify(sclient, "192.168.1.17:8443", "Your_Gotify_Key");
#define Serial Gotify // Hijack the Serial link !!! Warning: not all methods of Serial are yet supported

void setup()
{
  // put your setup code here, to run once:
  sclient.setFingerprint(serverfingerprint);
  // Alternatively, you can disable the fingerprint check:
  // sclient.setInsecure();
  Gotify.title("The default Gotify title for notifications");
  Serial.begin(115200);
  Serial.printf("\n");

  Serial.printf("Connecting to Wi-Fi...\n");
  Wifi.begin("Your SSID", "Your secret key");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  // If we reached that point, then we got connected to an access point as a client !
  Serial.println("WiFi connected");
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
  Gotify.send("You know what ?", "We are connectd !");
}
void loop(void) {
unsigned long now = millis();
Gotify.send("We are alive", String("Since : ")+String(now));
delay(5000);
}
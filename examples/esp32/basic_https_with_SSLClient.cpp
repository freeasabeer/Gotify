#include <Arduino.h>
#include <WiFi.h>
#include <SSLClient.h>
#include "Gotify.h"

// Put here the Root CA certificates of your Gotify server
const char RootCACert [] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIHFDCCBfygAwIBAgIQCLS/dX/bKN3zuMTJNXxaSTANBgkqhkiG9w0BAQsFADBP
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMSkwJwYDVQQDEyBE
aWdpQ2VydCBUTFMgUlNBIFNIQTI1NiAyMDIwIENBMTAeFw0yMjA0MDcwMDAwMDBa
Fw0yMzA0MDcyMzU5NTlaMGgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9y
bmlhMRYwFAYDVQQHEw1TYW4gRnJhbmNpc2NvMRUwEwYDVQQKEwxHaXRIdWIsIElu
Yy4xFTATBgNVBAMMDCouZ2l0aHViLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEP
ADCCAQoCggEBALyqZjatk2jnqiWmp6eusW70yJlreKz8mllyRSPxnIVeuwCHGzeQ
pGOOZkdRiBLcC2SWM3WgwQjBVBzqS1hWgoP5e6hzuXvGM3anlgJDE9dDUJfdC/Is
nzB4Q5Y4TU3FcRCUaK4GMoJGC0fu0fDbH927yKAnvdErG4u+jFSqIidwEaEfPWCC
o3xCyQLHTknXQ9aaDvU6GHNX0us6G+bjdErIwQtC56F0ke7biV0A/DWX5V+hVsVY
jY9JbYNx+KFjmUxLibccXzXs0pJ+a6Xa4OhhrFebPwS+SQA+gxTTvZotj4J5kf2l
nM9H+1whu6I5qPebhlTRTKpxdPm9V647Zj8CAwEAAaOCA9EwggPNMB8GA1UdIwQY
MBaAFLdrouqoqoSMeeq02g+YssWVdrn0MB0GA1UdDgQWBBRWmrM0shNZi0idiZiI
7l3ryIMwdDB7BgNVHREEdDByggwqLmdpdGh1Yi5jb22CDnd3dy5naXRodWIuY29t
gglnaXRodWIuaW+CCmdpdGh1Yi5jb22CCyouZ2l0aHViLmlvghVnaXRodWJ1c2Vy
Y29udGVudC5jb22CFyouZ2l0aHVidXNlcmNvbnRlbnQuY29tMA4GA1UdDwEB/wQE
AwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwgY8GA1UdHwSBhzCB
hDBAoD6gPIY6aHR0cDovL2NybDMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0VExTUlNB
U0hBMjU2MjAyMENBMS00LmNybDBAoD6gPIY6aHR0cDovL2NybDQuZGlnaWNlcnQu
Y29tL0RpZ2lDZXJ0VExTUlNBU0hBMjU2MjAyMENBMS00LmNybDA+BgNVHSAENzA1
MDMGBmeBDAECAjApMCcGCCsGAQUFBwIBFhtodHRwOi8vd3d3LmRpZ2ljZXJ0LmNv
bS9DUFMwfwYIKwYBBQUHAQEEczBxMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5k
aWdpY2VydC5jb20wSQYIKwYBBQUHMAKGPWh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0
LmNvbS9EaWdpQ2VydFRMU1JTQVNIQTI1NjIwMjBDQTEtMS5jcnQwCQYDVR0TBAIw
ADCCAX8GCisGAQQB1nkCBAIEggFvBIIBawFpAHYA6D7Q2j71BjUy51covIlryQPT
y9ERa+zraeF3fW0GvW4AAAGABfvdbAAABAMARzBFAiAGLk49aFP9ARwPXCa59WnI
f5jIU5eFmqR6/W3Zm38KiwIhAIp8FySKqbKk600uO4iPsS6TW8hJl67PprwXYMlr
o3wPAHcANc8ZG7+xbFe/D61MbULLu7YnICZR6j/hKu+oA8M71kwAAAGABfvdXQAA
BAMASDBGAiEAjFarHnzcbBvQ8//um0zVd4G3T5zbW4XSUIJSTc5JGo8CIQDaT5K8
pji9egTYSypP9XfRK+Z2wID3j43uuGjiKSOKyQB2ALNzdwfhhFD4Y4bWBancEQlK
eS2xZwwLh9zwAw55NqWaAAABgAX73YsAAAQDAEcwRQIhAO/PWksY7Zd7W5NJr3e4
xRkx8J6Qv7a33VA3tkm96k4WAiBshJWPE2BjKzuQ/KEfiKnvD4dDa3btkmcWlpiD
R8AvQDANBgkqhkiG9w0BAQsFAAOCAQEARtY8iVMqqBCXGZj2NRhpxA4eS2b/e/56
JhnRWGz3wxf0aRjbaZ2sUH3aHe1UDyg4jVPgnSLsGnBMmN5Rk32uiB/5v6/uRhCa
l26Yi9MYbeQpt0980MxT5hhv8bThRiNa77+oAOcrYMJEGIf2/9k0yoefblEZTR02
6UU6pkDhxjMtpyNRr+IdqQM/4lCM6nu8FZ/qaLltvta1Enq+jEwEObo/PoBoQJzJ
j7hcu7rkyPQIK1raQ9pK7uFJ2/FgtxIUuT+by06LnUp82VB7QxlniXO2R4XgDzWd
umlpkAFJQvZ+Sa2rSdjynrTDedjQIv3s1jH2Tvao5fR23tW2XAQhVg==
-----END CERTIFICATE-----
)CERT";

WiFiClient client;
SSLClient sclient(client);
Gotify Gotify(sclient, "192.168.1.17:8433", "Your_Gotify_Key");
#define Serial Gotify // Hijack the Serial link !!! Warning: not all methods of Serial are yet supported

void setup()
{
  // put your setup code here, to run once:
  sclient.setCACert(CACert);
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
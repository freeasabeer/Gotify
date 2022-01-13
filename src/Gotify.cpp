#include <WiFi.h>
#include <HTTPClient.h>
#include "Gotify.h"

Gotify::Gotify(String server, String key) {
  this->_server = server;
  this->_key = key;
  this->_title = String("");
}

void Gotify::begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert, unsigned long timeout_ms) {
  Serial.begin(baud, config, rxPin, txPin, invert, timeout_ms);
}

void Gotify::title(String title) {
  this->_title = title;
}
void Gotify::title(const char *title) {
  this->_title = String(title);
}

bool Gotify::send(String title, String msg, int priority) {
  HTTPClient http;
  bool status;

  if (!WiFi.isConnected())
    return false;

  http.begin(String("http://")+this->_server+String("/message?token=")+this->_key);
  http.addHeader("Content-Type", "application/json");
  uint16_t httpResponseCode;
  msg.replace("\n", "\\u000A");
  msg.replace("\r", "");
  //String extra = "\"extras\": {\"client::display\": {\"contentType\": \"text/plain\"}}";
  //String jsonMsg = "{"+extra+",\"message\": \""+msg+"\",\"title\": \""+((title=="")?this->_title:title)+"\",\"priority\": 5}";
  String jsonMsg = "{\"message\": \""+msg+"\",\"title\": \""+((title=="")?this->_title:title)+"\",\"priority\": 5}";
  Serial.println("JSON message: "+jsonMsg);
  httpResponseCode = http.POST(jsonMsg);

  if(httpResponseCode>0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
    status = true;
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    status = false;
  }
  http.end();
  return status;
}

size_t Gotify::printf(const char *format, ...)
{
  char loc_buf[64];
  char * temp = loc_buf;
  va_list arg;
  va_list copy;
  va_start(arg, format);
  va_copy(copy, arg);
  int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
  va_end(copy);
  if(len < 0) {
      va_end(arg);
      return 0;
  };
  if(len >= sizeof(loc_buf)){
      temp = (char*) malloc(len+1);
      if(temp == NULL) {
          va_end(arg);
          return 0;
      }
      len = vsnprintf(temp, len+1, format, arg);
  }
  va_end(arg);
  //len = write((uint8_t*)temp, len);
  send("", temp);

  if(temp != loc_buf){
      free(temp);
  }
  return len;
}

size_t Gotify::print(const __FlashStringHelper *ifsh)
{
  return print(reinterpret_cast<const char *>(ifsh));
}

size_t Gotify::print(const String &s)
{
  send("", s);
  return s.length();
}

size_t Gotify::print(const char str[])
{
  send("", String(str));
  return strlen(str);
}

size_t Gotify::print(char c)
{
  const char *str = &c;
  send("", str);
  return 1;
}


size_t Gotify::println(const __FlashStringHelper *ifsh)
{
  return print(reinterpret_cast<const char *>(ifsh));
}

size_t Gotify::println(const String &s)
{
  return print(s);
}

size_t Gotify::println(const char str[])
{
  return print(str);
}

size_t Gotify::println(char c)
{
  return print(c);
}

size_t Gotify::println(void)
{
  //return print("\r\n");
  return 2;
}


/*
size_t Gotify::write(uint8_t c)
{
    send("", String(c));
    return 1;
}

size_t Gotify::write(const uint8_t *buffer, size_t size)
{
    send("", String((const char *)buffer);
    return size;
}
*/

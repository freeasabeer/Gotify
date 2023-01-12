#if defined (ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
#elif defined (ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif
#include "Gotify.h"

#if defined(ESP32)
Gotify::Gotify(String server, String key, bool serial_fallback, bool use_mutex, const char* CAcert) {
  _server = server;
  _key = key;
  _title = String("");
  _serial_fallback = serial_fallback;
  _cacert = CAcert;
  _use_mutex = use_mutex;

  if (use_mutex) {
    _SafeSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( _SafeSerialSemaphore ) != NULL )
      xSemaphoreGive( ( _SafeSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }
}
#endif

Gotify::Gotify(WiFiClient &client, String server, String key, bool serial_fallback, bool use_mutex, bool https) {
  _client = &client;
  _server = server;
  _key = key;
  _title = String("");
  _serial_fallback = serial_fallback;
  _https = https;
  _use_mutex = use_mutex;
#if defined(ESP32)
  if (use_mutex) {
    _SafeSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( _SafeSerialSemaphore ) != NULL )
      xSemaphoreGive( ( _SafeSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }
#endif
}

#if defined(ESP32)
void Gotify::begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert, unsigned long timeout_ms) {
  Serial.begin(baud, config, rxPin, txPin, invert, timeout_ms);
}
#elif defined(ESP8266)
void Gotify::begin(uint32_t baud) {
  Serial.begin(baud);
}
#endif

void Gotify::isConnectedCB(bool (*cb)()) {
  _cb = cb;
}

void Gotify::title(String title) {
  this->_title = title;
}
void Gotify::title(const char *title) {
  this->_title = String(title);
}

bool Gotify::send(String title, String msg, int priority) {
  bool status;
  HTTPClient http;

#if defined(ESP32)
  if (_use_mutex)
    xSemaphoreTake( _SafeSerialSemaphore, portMAX_DELAY );
#endif

  if (_cb) {
    if (!_cb()) {
      if (this->_serial_fallback) {
        Serial.println(msg);
      }
      return false;
    }
  } else {
    if (!WiFi.isConnected()) {
      if (this->_serial_fallback) {
        Serial.println(msg);
      }
      return false;
    }
  }

  // Build the message
  msg.replace("\n", "\\u000A");
  msg.replace("\r", "");
  //String extra = "\"extras\": {\"client::display\": {\"contentType\": \"text/plain\"}}";
  //String jsonMsg = "{"+extra+",\"message\": \""+msg+"\",\"title\": \""+((title=="")?this->_title:title)+"\",\"priority\": 5}";
  String jsonMsg = "{\"message\": \""+msg+"\",\"title\": \""+((title=="")?this->_title:title)+"\",\"priority\": 5}";
  if (_debug) Serial.println("JSON message: "+jsonMsg);

#if defined(ESP32)
  if (_client) {
#endif
    if (_https)
      http.begin(*_client, this->_server, 443, String("/message?token=")+this->_key, true);
    else
      http.begin(*_client, this->_server, 80, String("/message?token=")+this->_key, false);
#if defined(ESP32)
  } else {
    if (_cacert)
      http.begin(String("https://")+this->_server+String("/message?token=")+this->_key, _cacert);
    else
      http.begin(String("http://")+this->_server+String("/message?token=")+this->_key);
  }
#endif
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonMsg);

  if(httpResponseCode>0) {
    String response = http.getString();
    if (_debug) {
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    status = true;
  } else {
    if (_debug) {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    if (this->_serial_fallback) {
      Serial.println(msg);
    }
    status = false;
  }
  http.end();



#if defined(ESP32)
  if (_use_mutex)
    xSemaphoreGive( _SafeSerialSemaphore );
#endif

  return status;
}

size_t Gotify::printf(const char *format, ...)
{
  char loc_buf[128];
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

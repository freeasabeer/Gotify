/*
#if defined (ESP32)
  #include <WiFi.h>
#elif defined (ESP8266)
  #include <ESP8266WiFi.h>
#endif
*/
#include <Client.h>
#include <ArduinoHttpClient.h>

#include "Gotify.h"

Gotify::Gotify(Client &client, String server, String key, bool serial_fallback, bool use_mutex) {
  _client = &client;
  _URLPath = "/message?token="+key;
  _title = String("");
  _serial_fallback = serial_fallback;
  _use_mutex = use_mutex;
  const char *s = server.c_str();
  char *c = strchr(s, ':');
  if (c) {
    *c++ = 0;
    _server = String(s);
    _port = strtol(c, NULL, 10);
  } else {
    _server = server;
    _port = 80;
  }
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
  httpbegin();
}
#elif defined(ESP8266)
void Gotify::begin(uint32_t baud) {
  Serial.begin(baud);
  httpbegin();
}
#endif
bool Gotify::httpbegin() {
  bool res = true;

  if(_http)
    delete _http;

  _http = new HttpClient(*_client, _server, _port);

  _http->connectionKeepAlive();

  return res;
}

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
  }

  // Build the message
  msg.replace("\n", "\\u000A");
  msg.replace("\r", "");
  //String extra = "\"extras\": {\"client::display\": {\"contentType\": \"text/plain\"}}";
  //String jsonMsg = "{"+extra+",\"message\": \""+msg+"\",\"title\": \""+((title=="")?this->_title:title)+"\",\"priority\": 5}";
  String jsonMsg = "{\"message\": \""+msg+"\",\"title\": \""+((title=="")?this->_title:title)+"\",\"priority\": 5}";
  if (_debug) Serial.println("JSON message: "+jsonMsg);

  const char *contentType = "application/json";
  int httpResponseCode;
  int i = 0;
  do {
  //_http->post(String("/message?token=")+_key, contentType, jsonMsg);
  _http->startRequest(_URLPath.c_str(), HTTP_METHOD_POST, contentType, jsonMsg.length(), (const byte *)jsonMsg.c_str());
  httpResponseCode = _http->responseStatusCode();
  if(httpResponseCode>0) {
    String response = _http->responseBody();
    if (_debug) {
      Serial.println(httpResponseCode);
      //Serial.println(response);
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
    if (_debug) Serial.println("Restarting http");
    httpbegin();
    i++;
  }
  } while((httpResponseCode <0)&&(i<5));
  //http.end();



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

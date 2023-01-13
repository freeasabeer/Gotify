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

  if (!_client->connected()) {
    _client->stop();
    Serial.println("Reconnect to server");
    if (!_client->connect(_server.c_str(), _port)) {
      Serial.println("Connection to "+_server+":"+String(_port)+" failed!");
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

    _client->println("POST "+_URLPath+" HTTP/1.1");
    _client->println("Host: "+_server+":"+_port);
    _client->println("User-Agent: Gotify/1.0.0");
    //_client->println("Connection: close");
    _client->println("Content-Type: application/json");
    _client->println("Content-Length: "+String(jsonMsg.length()));
    _client->println();
    _client->write((const byte *)jsonMsg.c_str(), jsonMsg.length());

    int returnCode = -1;
    bool canReuse = true;
    bool firstline = true;
    int size =  -1;
    while (_client->connected()) {
      if (_client->available()) {
        String headerLine = _client->readStringUntil('\n');
        if (firstline) {
          firstline = false;
          if(canReuse && headerLine.startsWith("HTTP/1.")) {
            canReuse = (headerLine[sizeof "HTTP/1." - 1] != '0');
          }
          int codePos = headerLine.indexOf(' ') + 1;
          returnCode = headerLine.substring(codePos, headerLine.indexOf(' ', codePos)).toInt();
        } else if(headerLine.indexOf(':')) {
          String headerName = headerLine.substring(0, headerLine.indexOf(':'));
          String headerValue = headerLine.substring(headerLine.indexOf(':') + 1);
          headerValue.trim();
          if(headerName.equalsIgnoreCase("Content-Length")) {
            size = headerValue.toInt();
          }
        }
        if (headerLine == "\r") {
          //Serial.println("headers received");
          break;
        }
      }
    }
    if (_debug) Serial.println(returnCode);
    if  (returnCode >0) {
      status = true;
      // if there are incoming bytes available
      // from the server, read them and print them:
      while (_client->available()) {
        char c = _client->read();
        //if (_debug) Serial.write(c);
      }
    } else {
      if (_debug) {
        Serial.print("Error on sending POST: ");
        Serial.println(returnCode);
      }
      if (_serial_fallback) {
        Serial.println(msg);
      }
      status = false;
    }

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

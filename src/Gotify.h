#include <Arduino.h>
//#include "Print.h"
#include <String.h>
#if defined (ESP32)
  #include <freertos/FreeRTOS.h>
  //#include <HTTPClient.h>
#elif defined (ARDUINO_ARCH_RP2040)
  #include <FreeRTOS.h>
  #include <semphr.h>
  //#include <HTTPClient.h>
#elif defined(ESP8266)
#pragma message("No mutex support")
  //#include <ESP8266HTTPClient.h>
#endif
#include <ArduinoHttpClient.h>

class Gotify//: public Print
{
public:
  Gotify(Client &client, String server, String key, bool serial_fallback = false, bool use_mutex = false);
  void title(String title);
  void title(const char *title);
  bool send(String title, String msg, int priority = 5);
#if defined(ESP32)
  void begin(unsigned long baud, uint32_t config = 134217756U, int8_t rxPin = (int8_t)(-1), int8_t txPin = (int8_t)(-1), bool invert = false, unsigned long timeout_ms = 20000UL);
#elif defined(ESP8266)
  void begin(uint32_t baud);//, SoftwareSerialConfig config = SWSERIAL_8N1);
#endif
  void isConnectedCB(bool (*cb)());
/*
  size_t write(uint8_t);
  size_t write(const uint8_t *buffer, size_t size);
  inline size_t write(const char * buffer, size_t size)
  {
      return write((uint8_t*) buffer, size);
  }
  inline size_t write(const char * s)
  {
      return write((uint8_t*) s, strlen(s));
  }
  inline size_t write(unsigned long n)
  {
      return write((uint8_t) n);
  }
  inline size_t write(long n)
  {
      return write((uint8_t) n);
  }
  inline size_t write(unsigned int n)
  {
      return write((uint8_t) n);
  }
  inline size_t write(int n)
  {
      return write((uint8_t) n);
  }
*/

  size_t printf(const char * format, ...)  __attribute__ ((format (printf, 2, 3)));

  size_t print(const __FlashStringHelper *);
  size_t print(const String &);
  size_t print(const char[]);
  size_t print(char);
/*
  size_t print(unsigned char, int = DEC);
  size_t print(int, int = DEC);
  size_t print(unsigned int, int = DEC);
  size_t print(long, int = DEC);
  size_t print(unsigned long, int = DEC);
  size_t print(long long, int = DEC);
  size_t print(unsigned long long, int = DEC);
  size_t print(double, int = 2);
  size_t print(const Printable&);
  size_t print(struct tm * timeinfo, const char * format = NULL);
*/

  size_t println(const __FlashStringHelper *);
  size_t println(const String &s);
  size_t println(const char[]);
  size_t println(char);
/*
  size_t println(unsigned char, int = DEC);
  size_t println(int, int = DEC);
  size_t println(unsigned int, int = DEC);
  size_t println(long, int = DEC);
  size_t println(unsigned long, int = DEC);
  size_t println(long long, int = DEC);
  size_t println(unsigned long long, int = DEC);
  size_t println(double, int = 2);
  size_t println(const Printable&);
  size_t println(struct tm * timeinfo, const char * format = NULL);
*/
  size_t println(void);

private:
  bool httpbegin();
  String _server;
  String _URLPath;
  String _title;
  bool _serial_fallback;
  bool _use_mutex;
  const char * _cacert;
#if defined(ESP32)
  volatile SemaphoreHandle_t _SafeSerialSemaphore = nullptr;
#endif
  bool _debug = true;
  Client *_client = nullptr;
  bool (*_cb)() = nullptr;
  HttpClient *_http = nullptr;
  int _port = 0;
};
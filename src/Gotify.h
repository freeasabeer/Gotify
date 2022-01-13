#include <Arduino.h>
//#include "Print.h"
#include <String.h>

class Gotify//: public Print
{
public:
  Gotify(String server, String key);
  void title(String title);
  void title(const char *title);
  bool send(String title, String msg, int priority = 5);
  void begin(unsigned long baud, uint32_t config = 134217756U, int8_t rxPin = (int8_t)(-1), int8_t txPin = (int8_t)(-1), bool invert = false, unsigned long timeout_ms = 20000UL);

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
  String _server;
  String _key;
  String _title;
};
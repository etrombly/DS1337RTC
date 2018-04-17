/*
 * DS1337RTC.h - library for DS1337 RTC
 * This library is intended to be uses with Arduino Time.h library functions
 */

#ifndef DS1337RTC_h
#define DS1337RTC_h

#include <TimeLib.h>

#define DS1337_CTRL_ID 0x68
#define CLOCK_ADDRESS 0x00
#define ALARM1_ADDRESS 0x07
#define ALARM2_ADDRESS 0x0B
#define STATUS_ADDRESS 0x0F
#define CONTROL_ADDRESS 0x0E
#define INTB 0
#define SQW 1

// library interface description
class DS1337RTC
{
  // user-accessible "public" interface
  public:
    DS1337RTC();
    static time_t get(int address);
    static void set(time_t t, int address);
    static time_t sync();
    static void read(tmElements_t &tm, int address);
    static void write(tmElements_t &tm, int address);
    static void enableAlarm(int address);
    static void disableAlarm(int address);
    static void resetAlarms();
    static void interruptSelect(int mode);
    static void freqSelect(int freq);

  private:
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
    static void startClock();
    static void stopClock();
};

extern DS1337RTC RTC;

#endif
 


/*
 * DS1337RTC.h - library for DS1337 RTC
  
  By Eric Trombly
  
  modified from DS1307RTC.h (c) Michael Margolis 2009
  This library is intended to be uses with Arduino Time.h library functions

  The library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  7 Jul 2011 - Initial release
 */

#include <Wire.h>
#include <WProgram.h>
#include "DS1337RTC.h"

byte controlRegister = 0x18; // default settings on start

DS1337RTC::DS1337RTC()
{
  Wire.begin();
}
  
// PUBLIC FUNCTIONS
time_t DS1337RTC::get(int address)   // Aquire data from buffer and convert to time_t
{
  tmElements_t tm;
  read(tm, address);
  return(makeTime(tm));
}

void  DS1337RTC::set(time_t t, int address) // Convert from time_t and save to rtc
{
  tmElements_t tm;
  breakTime(t, tm);
  stopClock();
  write(tm, address); 
  startClock();
}

time_t DS1337RTC::sync()  // Function to use for Time.h setSyncProvider
{
  return(get(CLOCK_ADDRESS));
}

void DS1337RTC::read( tmElements_t &tm, int address) // Aquire data from the RTC chip in BCD format
{
  int numberBytes;
  
  switch(address){
    case CLOCK_ADDRESS: numberBytes = tmNbrFields; break;
    case ALARM1_ADDRESS: numberBytes = 4; break;
    case ALARM2_ADDRESS: numberBytes = 3; break;
  }
  
  Wire.beginTransmission(DS1337_CTRL_ID);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(DS1337_CTRL_ID, numberBytes);
  
  if(address != ALARM2_ADDRESS){  // alarm 2 doesn't have a seconds field 
    tm.Second = bcd2dec(Wire.read() );
  }
  tm.Minute = bcd2dec(Wire.read() );
  tm.Hour =   bcd2dec(Wire.read() );
  if(address == ALARM1_ADDRESS || address == ALARM2_ADDRESS){  // the alarms don't have a wday,day,month,or year field
    tm.Day = bcd2dec(Wire.read() );
    tm.Month = 0;
    tm.Year = 0;
  }else{
    tm.Wday = bcd2dec(Wire.read() );
    tm.Day = bcd2dec(Wire.read() );
    tm.Month = bcd2dec(Wire.read() );
    tm.Year = y2kYearToTm((bcd2dec(Wire.read())));
  }
}

void DS1337RTC::write(tmElements_t &tm, int address)
{
  Wire.beginTransmission(DS1337_CTRL_ID);
  Wire.write(address); 
  
  if(address != ALARM2_ADDRESS){  // alarm 2 doesn't have a seconds field 
    Wire.write(dec2bcd(tm.Second));
  }
  Wire.write(dec2bcd(tm.Minute));
  Wire.write(dec2bcd(tm.Hour));
  if(address == ALARM1_ADDRESS || address == ALARM2_ADDRESS){  // the alarms don't have a wday,day,month,or year field
    Wire.write(dec2bcd(tm.Day));
  }else{
    Wire.write(dec2bcd(tm.Wday));   
    Wire.write(dec2bcd(tm.Day));
    Wire.write(dec2bcd(tm.Month));
    Wire.write(dec2bcd(tmYearToY2k(tm.Year)));   
  }
  Wire.endTransmission();
}

void DS1337RTC::enableAlarm(int address){  //turn on the A1IE or A2IE bit
  Wire.beginTransmission(DS1337_CTRL_ID);
  Wire.write(CONTROL_ADDRESS);  
  if(address == ALARM1_ADDRESS){
    controlRegister |= 0x01;
  }else{
    controlRegister |= 0x03;
  }
  
  Wire.write(controlRegister);
  Wire.endTransmission();
}

void DS1337RTC::disableAlarm(int address){  // turn off the A1IE or A2IE bit
  Wire.beginTransmission(DS1337_CTRL_ID);
  Wire.write(CONTROL_ADDRESS);  
  if(address == ALARM1_ADDRESS){
    controlRegister &= 0xFE;
  }else{
    controlRegister &= 0xFD;
  }
  
  Wire.write(controlRegister);
  Wire.endTransmission();
}

void DS1337RTC::resetAlarms(){  // reset the A1F and A2F bits
  Wire.beginTransmission(DS1337_CTRL_ID);
  Wire.write(STATUS_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();
}

void DS1337RTC::interruptSelect(int mode){  // set the INTCN bit, valid modes are INTB and SQW
  Wire.beginTransmission(DS1337_CTRL_ID);
  Wire.write(CONTROL_ADDRESS);
  if(mode == INTB){
    controlRegister |= 0x04;
  }else{
    controlRegister &= 0xFB;
  }
  
  Wire.write(controlRegister);
  Wire.endTransmission();
}

void DS1337RTC::freqSelect(int freq){  // set RS1 and RS2 bits, freq 0=1Hz, 1=4.096kHz, 2=8.192kHz, 3=32.768kHz
  Wire.beginTransmission(DS1337_CTRL_ID);
  Wire.write(CONTROL_ADDRESS);
  switch(freq){
    case 0: controlRegister &= 0xE7; break;
    case 1: controlRegister |= 0x08; break;
    case 2: controlRegister |= 0x10; break;
    case 3: controlRegister |= 0x18; break;
  }
  
  Wire.write(controlRegister);
  Wire.endTransmission();
}
// PRIVATE FUNCTIONS

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t DS1337RTC::dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t DS1337RTC::bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

void DS1337RTC::stopClock(){
  Wire.beginTransmission(DS1337_CTRL_ID);
  Wire.write(STATUS_ADDRESS);
  Wire.write(0x80);
  Wire.endTransmission();
}

void DS1337RTC::startClock(){
  Wire.beginTransmission(DS1337_CTRL_ID);
  Wire.write(STATUS_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();
}

DS1337RTC RTC = DS1337RTC(); // create an instance for the user


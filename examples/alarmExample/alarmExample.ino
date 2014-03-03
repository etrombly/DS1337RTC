#include <DS1337RTC.h>
#include <Time.h>
#include <Wire.h>
#define alarmPin 12

char *monthName[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void setup(){
  pinMode(alarmPin, INPUT);
  digitalWrite(alarmPin,HIGH);

  Serial.begin(9600);

  tmElements_t tmSet;
  tmSet.Year = 2014 - 1970;
  tmSet.Month = 3;
  tmSet.Day = 4;
  tmSet.Hour = 10;
  tmSet.Minute = 13;
  tmSet.Second = 0;
  RTC.set(makeTime(tmSet), CLOCK_ADDRESS); // set the clock

  tmSet.Second = 4;
  RTC.set(makeTime(tmSet), ALARM1_ADDRESS); // set the alarm for 4 seconds later

  RTC.enableAlarm(ALARM1_ADDRESS);

  RTC.freqSelect(1);  // set the squarewave freq on alarm pin b to 4.096kHz
}
  
void loop(){
  time_t clock = RTC.get(CLOCK_ADDRESS);
  time_t alarm = RTC.get(ALARM1_ADDRESS);  // get the time the alarm is set for
  tmElements_t clockSet;
  tmElements_t alarmSet;
  breakTime(clock, clockSet);
  breakTime(alarm, alarmSet);

  //print the time using Time.h
  Serial.print((int)clockSet.Day);
  Serial.print(" ");
  Serial.print((int)clockSet.Month);//monthName[month() - 1]);
  Serial.print(" ");
  Serial.print((int)(1970+clockSet.Year));//year());
  Serial.print(" ");
  Serial.print((int)clockSet.Hour);//hour());
  printDigits(clockSet.Minute);//minute());
  printDigits(clockSet.Second);//second());
  Serial.print("\t");

  // print the alarm time
  Serial.print("ALARM1: ");
  Serial.print((int)alarmSet.Day);
  Serial.print(" ");
  Serial.print((int)alarmSet.Hour);
  printDigits(alarmSet.Minute);
  printDigits(alarmSet.Second);
  Serial.print("\t");
  
  if(digitalRead(alarmPin) == HIGH){  //pin is set low when alarm is triggered
    Serial.println("ALARM off");
  }else{
    Serial.println("ALARM on");
    RTC.resetAlarms();
  }
  
  delay(1000);
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

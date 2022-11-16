#pragma once
#include "real_time.h"

  static iarduino_RTC time(RTC_DS1302,7,9,8); //RST,CLK,DAT Задаем правильно название нашего модуля

    RealTime::RealTime () {
      
    }
    
    char * RealTime::getDate() {
      return time.gettime("d.m.Y");
    }

    void RealTime::beginn() {
      time.begin();
      //time.period(1);
    }

    char * RealTime::getTime() {
      return time.gettime("H:i:s");
    }

    uint8_t RealTime::getYear(){
      time.gettime();
      return time.year;
    }
    uint8_t RealTime::getMonth(){
      time.gettime();
      return time.month;
    }
    uint8_t RealTime::getDay(){
      time.gettime();
      return time.day;
    }
    uint8_t RealTime::getHour(){
      time.gettime();
      return time.Hours;
    }
    uint8_t RealTime::getMinute(){
      time.gettime();
      return time.minutes;
        //return time.seconds;
    }
    uint8_t RealTime::getSeconds(){
      time.gettime();
      return time.seconds;
    }

    void RealTime::setTime(uint8_t minute, uint8_t hour) {
      time.settime(-1, minute, hour); 
    }

    void RealTime::setDate(uint8_t day, uint8_t month, uint8_t year) {
      time.settime(-1, -1,-1, day, month,year); 
    }

     void  RealTime::blinktime(uint8_t m){
      time.blinktime(m);
     }
     char * RealTime::getTime(char * s){
        return time.gettime(s);
     }

     int RealTime::getCurrentTimeMinutes(){
      time.gettime();
      return time.Hours * 60 + time.minutes;
     }


    

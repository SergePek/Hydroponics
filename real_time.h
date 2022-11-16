#pragma once
#include <iarduino_RTC.h>
#include <Arduino.h>

class RealTime {
	public:

    RealTime::RealTime();
    
		char * getDate();
    char * getTime();
    void setTime(uint8_t minute, uint8_t hour);
    void setDate(uint8_t day, uint8_t month, uint8_t year);
    void setDate();
    void beginn();
    uint8_t getYear();
    uint8_t getMonth();
    uint8_t getDay();
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSeconds();
    void blinktime(uint8_t m);
    char * getTime(char * s);
	  int getCurrentTimeMinutes();
	private:
   
};

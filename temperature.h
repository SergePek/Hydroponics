#pragma once
// класс опроса и фильтрации датчика
//#define ONE_WIRE_BUS 3  // пин ds18b20
#include <Arduino.h>
#include <DallasTemperature.h>

   
class Temperature {
  
  public:
  
    Temperature(DallasTemperature _sensors);
    void begin();
    void filter();
    float get();
    
  private:
    float tempReal;
    DallasTemperature sensors;
    float temp;
    uint8_t i,j,n;
    
};

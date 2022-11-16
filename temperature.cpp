#pragma once
#include "temperature.h"


//28 7C 03 43 98 01 02 A8

//28 37 04 43 98 FF 00 8D

//DeviceAddress WATHER = { 
//  0x28, 0x7C, 0x03, 0x43, 0x98, 0x01, 0x02, 0xA8 };  // адрес датчика 28D80443980F0019
//DeviceAddress AIR = { 
//  0x28, 0x37, 0x04, 0x43, 0x98, 0xFF, 0x00, 0x8D };
//  

Temperature::Temperature(DallasTemperature _sensors) {
   sensors = _sensors;
   
}

void Temperature::begin() {

  // термометр
  sensors.begin();
  sensors.setResolution(9);
  sensors.setWaitForConversion(false);  // асинхронное получение данных
}

void Temperature::filter() {  
  
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);  
    //sensors.requestTemperatures();

    if(temp < tempReal)
        i++;
    else
        i=0;
    if(temp == tempReal)
        j++;
    else
        j=0;
    if(temp > tempReal)
        n++;
    else
        n=0;

    if (i>1 || n>1 || j>1){
        tempReal = temp;
    }
}
      
float Temperature::get() {
  return tempReal;
}

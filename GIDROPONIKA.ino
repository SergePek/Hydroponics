#include "U8glib.h"
#include "rus6x12.h"
#include "button.h"
#include "real_time.h"
#include "timer.h"
#include <EEPROM.h>

#include <OneWire.h>
#include "temperature.h"

U8GLIB_ST7920_128X64_1X u8g(13, 11, 10);//E,R/W,RS
u8g_uint_t w, d;

#define KEY_NONE 0
#define KEY_PREV 1
#define KEY_NEXT 2
#define KEY_SELECT 3
#define KEY_BACK 4
#define WATH_STATE 22
#define SUN_STATE 24
//#define tempAir 0
//#define tempWather 1

uint8_t uiKeyPrev = 51;
uint8_t uiKeyNext = 49;
uint8_t uiKeySelect = 53;
uint8_t uiKeyBack = 47;

uint8_t minute_period_wather;
uint8_t minute_time_wather;

Button incBtn(uiKeyPrev);
Button decBtn(uiKeyNext);
Button selectBtn(uiKeySelect);
Button backBtn(uiKeyBack);
Button nextBtn(45);

uint8_t uiKeyCodeFirst = KEY_NONE;
uint8_t uiKeyCodeSecond = KEY_NONE;
uint8_t uiKeyCode = KEY_NONE;

RealTime realTime;

OneWire oneWire(33);
DallasTemperature sensors(&oneWire);

OneWire oneWiree(31);
DallasTemperature sensorss(&oneWiree);

Temperature dallasAir(sensors);
Temperature dallasWather(sensorss);

Timer timerDallas(2000);

uint8_t h =12;

struct MenuItem {
  char *title;
  struct MenuItem  *Next;
  struct MenuItem  *Previous;
  struct MenuItem  *Parent;
  struct MenuItem  *Child;
  void (*Handler)();
  bool edit;
};

MenuItem main_screen,a,b,c,dd,e,
          a_1, a_2, a_3;

MenuItem *pMenuItemCurrent;

MenuItem * handler;
uint8_t menu_current = 0;
uint8_t menu_redraw_required = 0;
uint8_t display_redraw_required = 1;
uint8_t last_key_code = KEY_NONE;

struct Data {
    int8_t hour_sun_on;
    int8_t minute_sun_on;

    int8_t hour_sun_off;
    int8_t minute_sun_off;

    int8_t hour_period_wather;
    int8_t minute_period_wather;

    int8_t hour_time_wather;
    int8_t minute_time_wather;

    bool wathering;
    bool suning;
};
Data data;

void(* resetFunc) (void) = 0;
////////////////////////////////////////////////////

void setTimersWather(){
   static uint8_t n;
   static bool saveData;
   menu_redraw_required = 0;

   if (  display_redraw_required != 0 ) {
    u8g.firstPage();
    do {
      u8g.setFont(rus6x12);
      u8g.setFontPosTop(); 
      w = u8g.getWidth();
      u8g.setDefaultForegroundColor();
      u8g.drawStr(35, 2, "ПОЛИВ ВОДЫ");
      
      u8g.setPrintPos(1, 14);
      u8g.print("КАЖДЫЕ: ");
      
      if(n==0){
        
        u8g.drawBox(48, 14, 13, 12);
        u8g.setDefaultBackgroundColor(); 
        u8g.setPrintPos(49, 14);

        u8g.print(data.hour_period_wather<10 ? "0" : "");
        u8g.print(data.hour_period_wather);
        
        u8g.setDefaultForegroundColor();
      } else {
        u8g.print(data.hour_period_wather<10 ? "0" : "");
        u8g.print(data.hour_period_wather);
      }
      u8g.print(":");
      
      if(n==1){
        u8g.drawBox(66, 14, 13, 12);
        u8g.setDefaultBackgroundColor(); 
        u8g.setPrintPos(67, 14);

        u8g.print(data.minute_period_wather<10?"0":"");
        u8g.print(data.minute_period_wather);
        
        u8g.setDefaultForegroundColor();
      } else {
        u8g.print(data.minute_period_wather<10?"0":"");
        u8g.print(data.minute_period_wather);
      }
      
      u8g.setPrintPos(30, 26);
      u8g.print("ВРЕМЯ РАБОТЫ");
      u8g.setPrintPos(1, 38);
      u8g.print("НАСОСА: ");
      
      if(n==2){
        
        u8g.drawBox(48, 38, 13, 12);
        u8g.setDefaultBackgroundColor(); 
        u8g.setPrintPos(49, 38);

        u8g.print(data.hour_time_wather<10?"0":"");
        u8g.print(data.hour_time_wather);
        
        u8g.setDefaultForegroundColor();
      } else {
        u8g.print(data.hour_time_wather<10?"0":"");
        u8g.print(data.hour_time_wather);
      }
      u8g.print(":");

      if(n==3){
        u8g.drawBox(66, 38, 13, 12);
        u8g.setDefaultBackgroundColor(); 
        u8g.setPrintPos(67, 38);

        u8g.print(data.minute_time_wather<10?"0":"");
        u8g.print(data.minute_time_wather);
        
        u8g.setDefaultForegroundColor();
      } else {
        u8g.print(data.minute_time_wather<10?"0":"");
        u8g.print(data.minute_time_wather);
      }

        if(n==4){
          if(saveData) {
          u8g.drawStr(0, 50, "СОХРАНЕНО");
          }
          else {
          u8g.drawStr(0, 50, "СОХРАНИТЬ?");
          }
          u8g.drawBox(64, 50, 15, 12);
          u8g.setDefaultBackgroundColor(); 
          u8g.setPrintPos(65, 50);
          u8g.print("ДА");
          u8g.setDefaultForegroundColor();
       
        } else {
          u8g.drawStr(0, 50, "СОХРАНИТЬ?");
          u8g.setPrintPos(65, 50);
          u8g.print("ДА");
        }
      } while( u8g.nextPage() );
      display_redraw_required = 0;
   }

  if(nextBtn.click()) {
    saveData = false;
    n++;
    if(n>4) n=0;
    display_redraw_required = 1;  
  }
  
  if(selectBtn.click()){
    if(n==4){
      EEPROM.put(0, data);
      saveData = true;
      display_redraw_required = 1; 
    }
  }

  if(decBtn.click()) {
    
    switch(n){
      case 0:
         data.hour_period_wather --;
         break;
      case 1:
         data.minute_period_wather --;
         break;
      case 2:
         data.hour_time_wather --;
         break;
      case 3:
         data.minute_time_wather --;
         break;
    }
    display_redraw_required = 1;  
  }
  if(incBtn.click()) {
    
     switch(n){
      case 0:
         data.hour_period_wather ++;
         break;
      case 1:
         data.minute_period_wather ++;
         break;
      case 2:
         data.hour_time_wather ++;
         break;
      case 3:
         data.minute_time_wather ++;
         break;
    }
      display_redraw_required = 1;
    }
    if(data.hour_period_wather > 23) data.hour_period_wather=0;
    if(data.hour_period_wather < 0) data.hour_period_wather=23;
    
    if(data.minute_period_wather > 59) data.minute_period_wather=0;
    if(data.minute_period_wather < 0) data.minute_period_wather=59;
    
    if(data.hour_time_wather > 23) data.hour_time_wather = 0;
    if(data.hour_time_wather < 0) data.hour_time_wather = 23;
    
    if(data.minute_time_wather > 59) data.minute_time_wather=0;
    if(data.minute_time_wather < 0) data.minute_time_wather=59;
}



void setDateAndTime(){
  
    menu_redraw_required = 0;  
    
    static uint8_t _year;
    static uint8_t _month;
    static uint8_t _day;
    static uint8_t _hour;
    static uint8_t _minute;

    _year = realTime.getYear();
    _month= realTime.getMonth();
    _day = realTime.getDay();
    _hour = realTime.getHour();
    _minute = realTime.getMinute();

   
  
  static uint8_t n;
 
  if(decBtn.click()) {
    
    switch(n){
      case 0:
         realTime.setTime(_minute, --_hour);
         break;
      case 1:
         realTime.setTime(--_minute, _hour);
         break;
      case 2:
         realTime.setDate(--_day, _month, _year);
         break;
      case 3:
         realTime.setDate(_day, --_month, _year);
         break;
      case 4:
         realTime.setDate(_day, _month, --_year);
         break;
    }
    display_redraw_required = 1;  
  }
  if(incBtn.click()) {
    
     switch(n){
      case 0:
         realTime.setTime(_minute, ++_hour);
         break;
      case 1:
         realTime.setTime(++_minute, _hour);
         break;
      case 2:
         realTime.setDate(++_day, _month, _year);
         break;
      case 3:
         realTime.setDate(_day, ++_month, _year);
         break;
      case 4:
         realTime.setDate(_day, _month, ++_year);
         break;
    }
    
    display_redraw_required = 1;  
  }
  if(nextBtn.click()) {
    Serial.println("НАЖАЛИ селект");
    n++;
    if(n>4) n=0;
    display_redraw_required = 1;  
  }
  u8g.firstPage(); 
  if (  display_redraw_required != 0 ) {
    u8g.firstPage();
    do {
      u8g.setFont(rus6x12);
      u8g.setFontPosTop(); 
      u8g.setDefaultForegroundColor();

      
      u8g.drawStr(10, 2, "УСТАНОВКА ВРЕМЕНИ");
      u8g.setPrintPos(15, 12);
      
      if(n==0){

        
        u8g.drawBox(13, 12, 15, 12);
        u8g.setDefaultBackgroundColor();

        u8g.print(_hour<10?"0":"");
        u8g.print(_hour);
        
        u8g.setDefaultForegroundColor();
      } else {
        u8g.print(_hour<10?"0":"");
        u8g.print(_hour);
      }
      u8g.setPrintPos(25, 12);
      u8g.print(" часов");
      u8g.print(":");

      u8g.setPrintPos(70, 12);
      
       if(n==1){

         u8g.drawBox(68, 12, 15, 12);
         u8g.setDefaultBackgroundColor();
         
         u8g.print(_minute<10?"0":"");
         u8g.print(_minute);
       
         u8g.setDefaultForegroundColor();
      } else {
         u8g.print(_minute<10?"0":"");
         u8g.print(_minute);
      }
     
      u8g.print(" минут");
      
      u8g.drawStr(20, 30, "УСТАНОВКА ДАТЫ");
      u8g.setPrintPos(20, 40);
      
      if(n==2){
         u8g.drawBox(18, 40, 15, 12);
         u8g.setDefaultBackgroundColor();

         u8g.print(_day<10?"0":"");
         u8g.print(_day);
        
         u8g.setDefaultForegroundColor();
      } else {
         u8g.print(_day<10?"0":"");
         u8g.print(_day);
      } 
      u8g.setPrintPos(33, 40); 
      u8g.print(".");
      
      if(n==3){
         u8g.drawBox(37, 40, 15, 12);
         u8g.setDefaultBackgroundColor();

        u8g.print(_month<10?"0":"");
        u8g.print(_month);
        
         u8g.setDefaultForegroundColor();
      } else {
        u8g.print(_month<10?"0":"");
        u8g.print(_month);
      } 
      
      u8g.setPrintPos(53, 40); 
       u8g.print(".");
       if(n==4){
         u8g.drawBox(57, 40, 30, 12);
         u8g.setDefaultBackgroundColor();
         
         u8g.print("20");
         u8g.print(_year);
       
         u8g.setDefaultForegroundColor();
      } else {
         u8g.print("20");
         u8g.print(_year);
      } 
      
       u8g.print(" год");
     
     
      u8g.drawStr(0, 55, "МЕНЮ");
      
    } while( u8g.nextPage() );
    display_redraw_required = 0;  
     
  }

}

void setTimersSun(){
    static uint8_t n;
    static bool saveData;
    menu_redraw_required = 0;

    if (  display_redraw_required != 0 ) {
    u8g.firstPage();
    do {
      u8g.setFont(rus6x12);
      u8g.setFontPosTop(); 
      w = u8g.getWidth();
      u8g.setDefaultForegroundColor();
      u8g.drawStr(1, 2, "ИКУСТВЕННОЕ ОСВЕЩЕНИЕ");
      u8g.drawStr(1, 14, "ВКЛЮЧЕНИЕ:");
      u8g.setPrintPos(65,14); 

      if(n==0){
        u8g.drawBox(64, 14, 13, 12);
        u8g.setDefaultBackgroundColor();
        u8g.print(data.hour_sun_on>=10 ? "" : "0");
        u8g.print(data.hour_sun_on);
        u8g.setDefaultForegroundColor();
         
      } else {
        u8g.print(data.hour_sun_on>=10 ? "" : "0");
        u8g.print(data.hour_sun_on);
      }
      u8g.print(":");

      if(n==1){
        u8g.drawBox(82, 14, 13, 12);
        u8g.setDefaultBackgroundColor();

        u8g.print(data.minute_sun_on>=10 ? "" : "0");
        u8g.print(data.minute_sun_on);
        
        u8g.setDefaultForegroundColor();
         
      } else {
        u8g.print(data.minute_sun_on>=10 ? "" : "0");
        u8g.print(data.minute_sun_on);
      }

      u8g.drawStr(0, 26, "ВЫКЛЮЧЕНИЕ:");
      
      u8g.setPrintPos(65, 26);
      if(n==2){
        u8g.drawBox(64, 26, 13, 12);
        u8g.setDefaultBackgroundColor();
        u8g.print(data.hour_sun_off>=10 ? "" : "0");
        u8g.print(data.hour_sun_off);
        u8g.setDefaultForegroundColor();
         
      } else {
        u8g.print(data.hour_sun_off>=10 ? "" : "0");
        u8g.print(data.hour_sun_off);
      }
      u8g.print(":");

      if(n==3){
        u8g.drawBox(82, 26, 13, 12);
        u8g.setDefaultBackgroundColor();
        u8g.print(data.minute_sun_off>=10 ? "" : "0");
        u8g.print(data.minute_sun_off);
        u8g.setDefaultForegroundColor();
      } else {
        u8g.print(data.minute_sun_off>=10 ? "" : "0");
        u8g.print(data.minute_sun_off);
      }
      if(n==4){
      if(saveData) {
        u8g.drawStr(0, 50, "СОХРАНЕНО");
      }
      else {
        u8g.drawStr(0, 50, "СОХРАНИТЬ?");
      }
       u8g.drawBox(64, 50, 15, 12);
       u8g.setDefaultBackgroundColor(); 
       u8g.setPrintPos(65, 50);
       u8g.print("ДА");
       u8g.setDefaultForegroundColor();
       
      } else {
       u8g.drawStr(0, 50, "СОХРАНИТЬ?");
       u8g.setPrintPos(65, 50);
       u8g.print("ДА");
      }
       
    } while( u8g.nextPage() );
    display_redraw_required = 0;
  }
  if(nextBtn.click()) {
    saveData = false;
    n++;
    if(n>4) n=0;
    display_redraw_required = 1;  
  }

  if(selectBtn.click()){
    if(n==4){
      EEPROM.put(0, data);
      saveData = true;
      display_redraw_required = 1; 
    }
  } 

  if(decBtn.click()) {
    
    switch(n){
      case 0:
         data.hour_sun_on --;
         break;
      case 1:
         data.minute_sun_on --;
         break;
      case 2:
         data.hour_sun_off --;
         break;
      case 3:
         data.minute_sun_off --;
         break;
    }
    display_redraw_required = 1;  
  }
  if(incBtn.click()) {
    
     switch(n){
      case 0:
         data.hour_sun_on ++;
         break;
      case 1:
         data.minute_sun_on ++;
         break;
      case 2:
         data.hour_sun_off ++;
         break;
      case 3:
         data.minute_sun_off ++;
         break;
      }
      display_redraw_required = 1;
    }
    if(data.minute_sun_off>59) data.minute_sun_off = 0;
    if(data.minute_sun_off<0) data.minute_sun_off = 59;
    
    if(data.minute_sun_on>59)  data.minute_sun_on = 0;
    if(data.minute_sun_on<0)  data.minute_sun_on = 59;
    
    if(data.hour_sun_off>23)  data.hour_sun_off = 0;
    if(data.hour_sun_off<0)  data.hour_sun_off = 23;
    
    if(data.hour_sun_on>23)  data.hour_sun_on = 0;
    if(data.hour_sun_on<0)  data.hour_sun_on = 23;
}

void editURL() {
   menu_redraw_required = 0;
  Serial.println("Вызван обработчиккккккк!");
 
  u8g.firstPage();
  static uint8_t i;
  if(incBtn.click()) {
    i++;
    display_redraw_required = 1;
  }
  //u8g_uint_t w, d;
  if (  display_redraw_required != 0 ) {
    u8g.firstPage();
    do {
      u8g.setFont(rus6x12);
      u8g.setFontPosTop(); 
      w = u8g.getWidth();
     
      u8g.setDefaultForegroundColor();
      u8g.drawStr(5, 2, "РЕДАКТИРОВАНИЕ URL");
      u8g.drawStr(5, 22, "НАЖИМАЙТЕ КНОПКИ ");
      u8g.setPrintPos(5, 40); 
      u8g.print(i);
      //Serial.print("Рисуем дисплей");
    } while( u8g.nextPage() );
    display_redraw_required = 0;
  }
}

void editData () {
  Serial.println("Вызван обработчик EDITDATA!");
}

//////////////////////////////////////////////////////////////////////////////////////////


void main_screen_draw() {
  static uint8_t lastTime, firstTime;
  menu_redraw_required = 0;
 
  
  if (  display_redraw_required != 0 ) {
    u8g.firstPage();
    do {
      u8g.setFont(rus6x12);
      u8g.setFontPosTop(); 
      w = u8g.getWidth();
      u8g.setDefaultForegroundColor();
      u8g.drawRFrame(0, 0,48,11,3);
      u8g.drawStr(3, 0, "СИСТЕМА");

      u8g.setPrintPos(63, 0);
      u8g.print(realTime.getTime("H:i:s"));
      //u8g.print(realTime.getSeconds()%2 == 1 ? ":" : " ");
      //u8g.print(realTime.getTime("i"));

      u8g.setPrintPos(0, 12);
      u8g.print("ПОЛИВ:");
      u8g.print(data.wathering ? "вкл " : "выкл");
      u8g.print(" СОЛНЦЕ:");
      u8g.print(data.suning ? "вкл " : "выкл");

      u8g.setPrintPos(0, 24);
      u8g.print("ТЕМП ВОЗДУХА: ");
      u8g.print(dallasAir.get());
      u8g.setPrintPos(0, 35);
      u8g.print("ТЕМП ВОДЫ: ");
      u8g.print(dallasWather.get());

      u8g.setPrintPos(0, 45);
      u8g.print(data.wathering ? "ВРЕМЯ ПОЛИВА: " :"БЕЗ ПОЛИВА: ");
      u8g.print(data.wathering ? minute_time_wather : minute_period_wather);
       u8g.print(" мин.");
//      minute_period_wather;
//      minute_time_wather;
      
      u8g.drawStr(0, 55, "НАЗАД");
      
    } while( u8g.nextPage() );
    display_redraw_required = 0;
  }

  lastTime = firstTime;
  firstTime = realTime.getSeconds();

  if(lastTime!=firstTime){
      display_redraw_required = 1;
    }

  if(incBtn.click()) {
    
    display_redraw_required = 1;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////

void uiStep(void) {
  uiKeyCodeSecond = uiKeyCodeFirst;
  if ( digitalRead(uiKeyPrev) == LOW )
    uiKeyCodeFirst = KEY_PREV;
  else if ( digitalRead(uiKeyNext) == LOW )
    uiKeyCodeFirst = KEY_NEXT;
  else if ( digitalRead(uiKeySelect) == LOW )
    uiKeyCodeFirst = KEY_SELECT;
  else if ( digitalRead(uiKeyBack) == LOW )
    uiKeyCodeFirst = KEY_BACK;
  else 
    uiKeyCodeFirst = KEY_NONE;
  
  if ( uiKeyCodeSecond == uiKeyCodeFirst )
    uiKeyCode = uiKeyCodeFirst;
  else
    uiKeyCode = KEY_NONE;
}

void drawMenu() {
  uint8_t i=0;
  //u8g_uint_t w, d;
  MenuItem *pMenuItem;
  u8g.setFont(rus6x12);
  u8g.setFontPosTop(); 
 
  w = u8g.getWidth();
  pMenuItem = pMenuItemCurrent;
  
  while(pMenuItem->Previous != NULL){
    pMenuItem = pMenuItem->Previous;
  }
  
  while(1){ 
    d = (w-u8g.getStrWidth(pMenuItem->title))/2;
    u8g.setDefaultForegroundColor();
    if ( pMenuItem->title == pMenuItemCurrent->title ) {
      u8g.drawBox(0, i*h, w, h);
      u8g.setDefaultBackgroundColor();
    }
    u8g.drawStr(d, i*h+2, pMenuItem->title);
 
    pMenuItem = pMenuItem->Next;
    i++;
    if(!pMenuItem){
      break;
      i=0;
    }
 }
}

 void updateMenu(void) {
   if ( uiKeyCode != KEY_NONE && last_key_code == uiKeyCode ) {
    return;
  }
  last_key_code = uiKeyCode;
  
  switch ( uiKeyCode ) {
    
    case KEY_NEXT:  if(pMenuItemCurrent->edit) break;
      
      if(pMenuItemCurrent->Next != NULL) {
        pMenuItemCurrent = pMenuItemCurrent->Next;
      }
      menu_redraw_required = 1;
      break;
      
    case KEY_PREV:  if(pMenuItemCurrent->edit) break;
    
      if(pMenuItemCurrent->Previous != NULL) {
        pMenuItemCurrent = pMenuItemCurrent->Previous;
      }
      menu_redraw_required = 1;
      break;
      
    case KEY_SELECT:  if(pMenuItemCurrent->edit) break;
    
      if(pMenuItemCurrent->Child != NULL){
        pMenuItemCurrent = pMenuItemCurrent->Child;
        menu_redraw_required = 1;
      } 
      else if(pMenuItemCurrent->Handler != NULL){
          pMenuItemCurrent->edit = true;
          display_redraw_required = 1;     
      }
      
      break;

    case KEY_BACK:
    
      if(pMenuItemCurrent->edit){
            pMenuItemCurrent->edit = false;
        } 
             
      if(pMenuItemCurrent->Parent != NULL) {
          pMenuItemCurrent = pMenuItemCurrent->Parent;
      }
      menu_redraw_required = 1;
      break;
  }
} 

void wathering(){

//  static uint8_t minute_period_wather;
//  static uint8_t minute_time_wather;
  static uint8_t last_minute;
  
  if(realTime.getMinute() != last_minute){
    
    last_minute = realTime.getMinute();
    
    if(!data.wathering) {
      if(minute_period_wather++ >= data.minute_period_wather + data.hour_period_wather *60){
         Serial.print("ВКЛЮЧЕН ПОЛИВ ЧЕРЕЗ:  ");
         Serial.println(minute_period_wather);
        minute_period_wather = 0;
        data.wathering = true;       
      }
    }

    if(data.wathering){
      
      if(minute_time_wather++ >= data.minute_time_wather + data.hour_time_wather *60){
       Serial.print("ПОЛИВ ВЫКЛЮЧЕН ЧЕРЕЗ: ");
       Serial.println(minute_time_wather);
        minute_time_wather = 0;
        data.wathering = false;
      }
    }
    
    //Serial.println(realTime.getMinute());
    data.wathering ? digitalWrite(WATH_STATE,HIGH) : digitalWrite(WATH_STATE,LOW);
  }
}


void suning(){

  static int timeOn;
  static int timeOff;
  
  timeOn = data.hour_sun_on*60 + data.minute_sun_on;
  timeOff = data.hour_sun_off*60 + data.minute_sun_off;
//  Serial.print("ВРЕМЯ ВКЛЮЧЕНИЯ   ");
//  Serial.println(timeOn);
//  Serial.print("ВРЕМЯ ОТКЛЮЧЕНИЯ   ");
//  Serial.println(timeOff);
  
  if(timeOn < timeOff) {
//    Serial.print("                         ЗАШЛИ  ");
//    Serial.println(realTime.getCurrentTimeMinutes());
  
    if(realTime.getCurrentTimeMinutes() >= timeOn && realTime.getCurrentTimeMinutes() < timeOff){
      data.suning = true;
      //Serial.println("СОЛНЦЕ ВКЛЮЧЕНО!!!");
    }
    else {
      data.suning = false;
    }
  } else {
    if( realTime.getCurrentTimeMinutes() < timeOn && realTime.getCurrentTimeMinutes() > timeOff){
        data.suning = false;
    }else{
        data.suning = true;
        
    }
    
  }
  
  data.suning ? digitalWrite(SUN_STATE,HIGH) : digitalWrite(SUN_STATE,LOW);
}

void setup() {
  
 Serial.begin(9600);  
 realTime.beginn();
 dallasAir.begin();
 dallasWather.begin();
 delay(2000);
 pinMode(WATH_STATE,OUTPUT);
 pinMode(SUN_STATE,OUTPUT);
 
//  data.hour_period_wather=0;
//  data.minute_period_wather=33;
//
//  data.hour_time_wather=0;
//  data.minute_time_wather=5;
// EEPROM.put(0, data);
 EEPROM.get(0, data);
 
                    //title;   Next;   Previous;   Parent;   Child;  Handler, edit;
 
 main_screen = {"ГЛАВНОЕ ОКНО", &a, NULL, NULL, NULL, main_screen_draw, true};
 
 a = {"НАСТРОЙКИ", &b, &main_screen, NULL, &a_1,  NULL, false  };
 
      a_1 = {"ДАТА И ВРЕМЯ",    &a_2,   NULL,   &a,  NULL,    setDateAndTime,     false  };
      a_2 = {"СОЛНЕЧНЫЙ СВЕТ",   &a_3,   &a_1,   &a,  NULL,    setTimersSun,     false  };
      a_3 = {"ПОЛИВ ВОДЫ",     NULL,   &a_2,   &a,  NULL,    setTimersWather,  false  };
          
 b = {"НЕ РЕАЛИЗОВАНО_1",        &c,     &a, NULL, NULL,  NULL, false  };
 c = {"НЕ РЕАЛИЗОВАНО_2",        &dd,     &b, NULL, NULL,  NULL, false  };
 dd = {"БУДИЛЬНИК",        NULL,   &c, NULL, NULL,  NULL, false  };

 pMenuItemCurrent = &main_screen;
 
  u8g_uint_t w, d;
  u8g.firstPage();
  do  {
     u8g.setFont(rus6x12);
    u8g.setFontPosTop(); 
    w = u8g.getWidth();
    d = (w-u8g.getStrWidth("ЗАГРУЗКА"))/2;
    u8g.setDefaultForegroundColor();
    u8g.drawStr(d, 2, "ЗАГРУЗКА");
    
  } while( u8g.nextPage() );
 delay(500);
}

    
void loop() {
  uiStep();    
  
  if (  menu_redraw_required != 0 ) {    
    u8g.firstPage();
    do  {
      drawMenu();
    } while( u8g.nextPage() );
    menu_redraw_required = 0;
  }
  updateMenu();   
  
  if(pMenuItemCurrent->edit) {
        //Serial.println("Вызываем обработчик");
        display_redraw_required = 1;
        pMenuItemCurrent->Handler ();
  }
  
  if(timerDallas.ready()) {
    dallasAir.filter();
    dallasWather.filter();
  }
  
  wathering();
  suning();
}

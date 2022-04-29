//Header files
//Temp 
#include <Wire.h>
#include <SFE_BMP180.h>

//LCD
#include <LiquidCrystal.h>

//Variables 

//Salinity 
int analogPin = 0;     // potentiometer wiper (middle terminal) connected to analog pin 0
int val = 0;           // variable to store the value read

//Temp
SFE_BMP180 bmp180;

//LCD 
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

typedef struct Task
{
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int(*TickFct)(int);
}task;

const unsigned short numTasks = 2;
Task tasks[numTasks];

//Salinity SM
int SALTEST_SM1();
enum SM1_STATES{SM1_Init, SM1_Wait} SM1_State;

int SALTEST_SM1(int SM1_State)
{
   //conditions
   switch(SM1_State)
   {
    case SM1_Init:
    SM1_State = SM1_Wait;
    break;

     case SM1_Wait:
     SM1_State = SM1_Wait;
     break;

     default:
     SM1_State = SM1_Wait;
     break;
     
   } 

    //actions
    switch(SM1_State)
   {
     case SM1_Init:
      break;

     case SM1_Wait:

      val = analogRead(analogPin);    // read the input pin
      lcd.setCursor(0, 0);
      lcd.print("Salinity: ");
      lcd.println(val);             // debug value
      break;

     default:
      break; 
    }
    
}

int TEMPTEST_SM2();
enum SM2_STATES{SM2_Init, SM2_Wait} SM2_State;

int TEMPTEST_SM2(int SM2_State)
{
   //conditions
   switch(SM2_State)
   {
    case SM2_Init:
    SM2_State = SM2_Wait;
    break;

     case SM2_Wait:
     SM2_State = SM2_Wait;
     break;

     default:
     SM2_State = SM2_Wait;
     break;
     
   } 

    //actions
    switch(SM2_State)
   {
     case SM2_Init:
      break;

     case SM2_Wait:
      char status;
      double T, P;
      bool success = false;
    
      status = bmp180.startTemperature();
    
      if (status != 0) 
      {
        delay(1000);
        status = bmp180.getTemperature(T);
    
        if (status != 0) 
        {
          status = bmp180.startPressure(3);
    
          if (status != 0) 
          {
            delay(status);
            status = bmp180.getPressure(P, T);
    
            if (status != 0) 
            {
              //Serial.print("wew");
              lcd.setCursor(0, 1);
              lcd.print("P:");
              lcd.print(P);
              lcd.println("hPa");

              lcd.setCursor(11, 1);
              lcd.print(" T:");
              lcd.print(T);
              lcd.println(" C");
            }
          }
        }
      }
      break;

     default:
      break; 
    }
    
}



void setup() 
{

    unsigned char i=0;
    //lcd
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);

    //Task 1 Salinity
    tasks[i].state= SM1_Init;
    tasks[i].period=1000;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &SALTEST_SM1;
    i++;

    //Task 2 Temp
    bool success = bmp180.begin();
    tasks[i].state= SM2_Init;
    tasks[i].period=1000;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TEMPTEST_SM2;
    i++;

    
    


    
  // put your setup code here, to run once:
  Serial.begin(9600);
  
//  //LCD screen
//  // set up the LCD's number of columns and rows:
//  lcd.begin(16, 2);
//  // Print a message to the LCD.
//  lcd.print("hello, world!");
//
//  //Temp
//  // Start up the library
//  sensors.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned int i;
  for (i = 0; i < numTasks; ++i) 
  {
    if ((millis() - tasks[i].elapsedTime) >= tasks[i].period) 
    {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis();
     }
  }


}

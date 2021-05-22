#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 4, 5, 6, 7);

//global variables for time:
int hour = 0;
int minute = 0;
int sec = 0;
bool isAM = true; //stores the part of the day - AM/PM 

//global variables for date:
int day = 2;
int month = 12;
int year = 2020;
bool isDayPassed = false; //true if the date shoud be changed

//global variables for temperature:
const int tempPin = A1;
float tempC = 0;
float tempF = 0;
bool isCel = true; //true if celcuis should be displayed

//global variables for time accurency
//help the loop to always be 1 sec 
long now = 0; 
long past = 1000;

//global variables for mode changing:
const int potPin = A0;
int potVal = 0;

void setup() {
  lcd.begin(16, 2); // output
  
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(potPin, INPUT);
  pinMode(tempPin, INPUT);

  //interrupt methods for buttons 1 and 2:
  attachInterrupt(0, button1Interrupt, FALLING);
  attachInterrupt(1, button2Interrupt, FALLING);

}

void loop() {
  now = millis(); // now equals the time since the arduino is 
                  // is turned on(in milliseconds)
  potCheck(); // updates the mode
  calcTemp(); // updates the temperature
  displayAll(); // displays the new values
  changeTime(); // updates time with +1 second

  //delays the loop until a second has passed
  while(now - past < 0){ 
    now = millis();
  }
  past = past + 1000;

}

//updates time with +1 sec:
void changeTime(){
  sec++;
  if(sec > 59){ 
    sec = 0;
    minute++;
  }
  if(minute > 59){
    minute = 0;
    raiseHours();
  }
  
}

//dispays the time on the lcd:
void printTime(){
  printNum(hour);
  lcd.print(":");
  printNum(minute);
  lcd.print(":");
  printNum(sec);
  if(isAM){
    lcd.print("AM");
  }else{
    lcd.print("PM");
  }

}

//changes hours with +1:
void raiseHours(){
  hour++;
  if(isAM){ // AM starts with 00:00 AM
    if(hour > 11){ // and ends with 11:59 AM
      isAM = false; 
    }
  }
  else{
    if(hour > 12){ //PM starts with 12:00 PM
      hour = 1; // then is 01:00 PM
    }
    else if(hour > 11){// and ends with 11:59 PM
      hour = 0;
      isAM = true;
      changeDate(); // changes date if a day has passed
    }
  }
}

// changing hours manually with button 2:
void changeHours(){
  raiseHours();
  displayAll();
}

// changing minutes manually with button 1:
void changeMins(){
 minute++;
 sec = 0; // always sets the seconds to 0
        // if minutes are changed manually
  if(minute > 59){
    minute= 0;
  }
  displayAll();
}

// prints 1 digit numbers with 0 before them:
void printNum(int n){
  if(n < 10){
    lcd.print("0");
  }
  lcd.print(n);
}

//displays the date on the lcd:
void printDate(){
  printNum(day);
  lcd.print(".");
  printMonth();
  lcd.print(".");
  lcd.print(year);
}

//changes the date with +1 day:
void changeDate(){
  int maxDay = getMaxDay(); // the maximum number of days
                            // for the according month
  day++;
  if(day > maxDay){
    day = 1;
    month++;
  }
  if(month > 12){
    month = 1;
    year++;
  }
}

//returns the maximum number of days for the current value
// of the global variable month:
int getMaxDay(){
  int maxDay = 31;
  bool isLeap = leapCheck(); // true if the year is leap
  if(month == 4 || month == 6 || month == 8 || month == 11){
    maxDay = 30;
  }
  else if(month == 2){
    if(isLeap){
      maxDay = 29;
    }
    else{
      maxDay = 28;
    }
  }
  return maxDay;
}

//returns true if the value of the global variable year is leap:
bool leapCheck(){
  if( year % 4 == 0 && ( ( year % 100 != 0 ) || 
  ( year % 400 == 0 ))) { 
    return true;
  } 
  else {
    return false;
  }

}

//diplays the value if the global var month in a 3-char format:
void printMonth(){
  switch(month){
    case 1:
    lcd.print("JAN");
    break;
    case 2:
    lcd.print("FEB");
    break;
    case 3:
    lcd.print("MAR");
    break;
    case 4:
    lcd.print("APR");
    break;
    case 5:
    lcd.print("MAY");
    break;
    case 6:
    lcd.print("JUN");
    break;
    case 7:
    lcd.print("JUL");
    break;
    case 8:
    lcd.print("AUG");
    break;
    case 9:
    lcd.print("SEP");
    break;
    case 10:
    lcd.print("OCT");
    break;
    case 11:
    lcd.print("NOV");
    break;
    case 12:
    lcd.print("DEC");
    break;
    default:
    lcd.print("ERR");
    break;
  }
}

//changes days manually using button 1 with +1 day:
void changeDays(){
  int maxDay = getMaxDay();
  day++;
  if(day > maxDay){
    day = 1;
  }
  displayAll();
}

//changes months manually using button 2 with +1 month;
void changeMonths(){
  month++;
  if(month > 12){
    month = 1;
  }
  displayAll();
}

//changes year manually using button 1 with +1:
void raiseYear(){
  year++;
  displayAll();
}

//changes year manually using button 2 with -1:
void lowerYear(){
  year--;
  displayAll();
}

//updates the value of the pot. to determine the mode of edit:
void potCheck(){
  potVal = analogRead(potPin);

  //maps the value of the pot to be in range: 1 - 4.
  potVal = map(potVal, 0, 1023, 1, 4);
}

//modes:
//1 - change time
//2 - change date
//3 - change year
//4 - change temp

//displays the temp on the lcd:
void printTemp(){
  // determines if the user wants the data in C or F
  if(isCel){
    lcd.print(tempC);
    lcd.setCursor(15, 0);
    lcd.print("C");
  }
  else{
    lcd.print(tempF);
    lcd.setCursor(15, 0);
    lcd.print("F");
  }
  
}


//displays what the user can edit with the buttons
//based on the potVal:
void printMode(){
  switch(potVal){
    case 1:
    lcd.print("TIME");
    break;
    case 2:
    lcd.print("DATE");
    break;
    case 3:
    lcd.print("YEAR");
    break;
    case 4:
    lcd.print("TEMP");
    break;
    default:
    lcd.print("ERR");
    break;
  }
}

//displays of the data on the lcd screen
//and sets the cursor on the apropriate places:
void displayAll(){
  lcd.clear();
  lcd.setCursor(0, 0);
  printTime();
  lcd.setCursor(11, 0);
  printTemp();
  lcd.setCursor(0, 1);
  printDate();
  lcd.setCursor(12, 1);
  printMode();
  
}

//interrupt method for button 1:
// determines what action to do depending on the potVal.
void button1Interrupt(){
  switch(potVal){
    case 1:
    changeMins();
    break;
    case 2:
    changeDays();
    break;
    case 3:
    raiseYear();
    break;
    case 4:
    switchTempC();
    break;
    default:
    return;
    break;
  }
}

//interrupt method for button 2:
// determines what action to do depending on the potVal.
void button2Interrupt(){
  switch(potVal){
    case 1:
    changeHours();
    break;
    case 2:
    changeMonths();
    break;
    case 3:
    lowerYear();
    break;
    case 4:
    switchTempF();
    default:
    return;
    break;
  }
}

//calculate the room temperature using TMP36:
void calcTemp(){
  float tempVal = analogRead(tempPin);
  float volt = (tempVal / 1024.0) * 5.0; // converts data to volts
  tempC = (volt - 0.5) * 100;// converts volts to celsius
  tempF = (tempC * 9.0 / 5.0) + 32.0;// converts celsius to F

}

//change temp manually to C with button 1:
void switchTempC(){
  isCel = true;
}

//change temp manually to F with button 2:
void switchTempF(){
  isCel = false;
}

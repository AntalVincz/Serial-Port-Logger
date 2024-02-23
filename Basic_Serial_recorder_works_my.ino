//Simple serial port logger
//SD CARD module connecting to MOSI, MISO, SCK, and SS PINS
// SDCARD LED ATMEGA PIN port 13 ---- 680Ohm RESISTOR ---- GND
// ACT LED ATMEGA PIN port 14 ------ 680Ohm RESISTOR ---- GND
// PUSH BUTTON ATMEGA port 1 ----- GND
// I2C liquid Crystal LCD ATMEGA SDA/SCL

// SPI and SD libraries. SPI for connecting SD card to SPI bus.
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

//ATMEGA644 pins
const int sdPin = 4; //real pin 5 SSPIN!!!
int sdcardLED = 13;
int actLED = 14;
const int buttonPin = 1;
SoftwareSerial mySerial(10, 11); // RX, TX

/*ATMEGA168 pins
const int sdPin = 10; //real pin 16
int sdcardLED = 8;  //real pin 14
int actLED = 9; //real pin 15
const int buttonPin = 7; //real pin 13
SoftwareSerial mySerial(4, 5); // RX, TX pin 4 and 5
*/

LiquidCrystal_I2C lcd(0x3F,20,4);

byte byteRead;
int oldfilenumber = 0;
char newfilename[] = "LOG00000.TXT";
int col = 0;
int row = 1;
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin
// File variable
File logFile;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
int status;


void setup() {
  // Serial output for when connected to computer
  Serial.begin(9600);
  mySerial.begin(1200);
  lcd.init();
  lcd.backlight();
  pinMode(sdcardLED, OUTPUT);
  pinMode(actLED, OUTPUT);
  digitalWrite(actLED, LOW);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.print("Initializing SD card...");
  lcd.setCursor(1,0);
  lcd.print("Serial port Logger");
  lcd.setCursor(5,1);
  lcd.print("Made by Tony");
  lcd.setCursor(8,3);
  lcd.print("2024");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Initializing SD card");
  if(SD.begin(sdPin)) {
    Serial.println("Initialization done.");
    digitalWrite(sdcardLED, HIGH);
    lcd.setCursor(0,1);
    lcd.print("Initialization done.");
    delay(1000);
    makenewfile();
   }
  else {Serial.println("Initialization fail.");
  digitalWrite(sdcardLED, LOW);
  lcd.setCursor(0,1);
    lcd.print("Initialization fail.");
  }
  
 }

void loop() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        lcd.clear();
      lcd.setCursor(0,0);
        lcd.print("Create a new logFile");
        lcd.setCursor(0,1);
        lcd.print("Done");
      col = 0; 
      makenewfile();
      lcd.setCursor(0,2);
      lcd.print(newfilename);
      lcd.setCursor(0,3);
      lcd.print("LogFile open done");
      delay(2000);
      lcd.clear();
      
      }
    }
  }
  lastButtonState = reading;
  
  logFile = SD.open(newfilename, FILE_WRITE);
  if (logFile) {
    while(mySerial.available()>0)
    {
    byteRead=mySerial.read();
    logFile.println(byteRead,HEX);
    // write temps to SD card
    //logFile.write(byteRead);
    digitalWrite(actLED, HIGH);
    delay(10);
    digitalWrite(actLED, LOW);
    //Serial.print(byteRead,HEX);
    Serial.print("col:");
    Serial.print(col);
    Serial.print("  row:");
    Serial.print(row);
    Serial.println();
    lcd.setCursor(0,0);
    lcd.print("Income data.....");
    delay(250);
    lcd.setCursor(col, row);
    lcd.print(byteRead,HEX);
    col = col+2;
    if (col > 18 && row <2){
      col = col-20;
      row = 2;
    }
    else if (col > 18 && row == 2){
    col = col-20;
    row = 3;
    }
    else if (col > 18 && row == 3){
      lcd.clear();
      col=0;
      row=1;
    }
  }
    logFile.close();
 }
    
}


void makenewfile(){
  int newfilenumber = oldfilenumber + 1;
  oldfilenumber = newfilenumber;
        //There is a max of 65534 logs
        if(newfilenumber > 65533) 
        {
            printf(("!Too many logs:2!"));
            return; //Bail!
        }

        //Splice the new file number into this file name
        sprintf(newfilename, "LOG%05d.TXT", newfilenumber); 
        //lcd.reset;
        Serial.print(newfilename);
        Serial.println();
        lcd.setCursor(0,2);
        lcd.print(newfilename);
        //logFile = SD.open(newfilename, FILE_WRITE);
        Serial.print("LogFile open done");
        Serial.println();
        lcd.setCursor(0,3);
        lcd.print("LogFile open done");
        delay(2000);
        lcd.clear();
        //logFile.write("byteRead");
        //logFile.close();
 }

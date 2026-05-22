#include <Wire.h>

#include <Stepper.h>

#include <HX711.h>

#define DT 4
#define SCK 5

HX711 scale;
//float calibration_factor = 19690.822265;       // 25 lbs setpoint
float calibration_factor = 20425.675781;       // 75 lbs setpoint
//float calibration_factor = 20554.382812;       // 125 lbs setpoint

//#include "Adafruit_LiquidCrystal.h"
//Adafruit_LiquidCrystal lcd(0x20);

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

const int RunPin = A1;
const int ComPin = A2;

const int limitPin = 7;

bool homingComplete = false;

const int stepsPerRevolution = 200;

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

int x;
float c, w;
float targetForce;
String receivedString; 


void setup() {

  scale.begin(DT, SCK);  
  scale.set_scale(calibration_factor);

  //scale.set_offset(-16839);                     // 25 lbs setpoint
  scale.set_offset(-16871);                     // 75 lbs setpoint
  //scale.set_offset(-16584);                     // 125 lbs setpoint
  scale.tare();

  myStepper.setSpeed(100);

  pinMode(RunPin, INPUT);
  pinMode(ComPin, OUTPUT);
  
  pinMode(limitPin, INPUT_PULLUP);
  
  //lcd.begin(20, 4);
  //lcd.setBacklight(HIGH);

  lcd.backlight();
  lcd.init();

  Serial.begin(115200);

}

void loop() {

  Serial.available();

  if (Serial.available()){
    receivedString = Serial.readString();
    targetForce = receivedString.toFloat();    
    c = scale.get_units();
    scale.tare();
    lcd.setCursor(0,0);
    lcd.print("TF: ");
    lcd.setCursor(4,0);
    lcd.print(targetForce, 1);
    
    x = analogRead(RunPin);
    
    while (x > 250){
      
      begin();
    }  
  }
}

void begin(){

    x = analogRead(RunPin);

    c = scale.get_units();
    w = (c * 2.20462);
    lcd.setCursor(0,2);
    lcd.print("CF: ");
    lcd.setCursor(4,2);
    lcd.print(w,1);

    if (w < targetForce){
      myStepper.step(stepsPerRevolution);
      lcd.setCursor(0,2);
      lcd.print("CF: ");
      lcd.setCursor(4,2);
      lcd.print(w,1); 
    }
    if (w > targetForce){
      myStepper.step(-1);
      lcd.setCursor(0,2);
      lcd.print("CF: ");
      lcd.setCursor(4,2);
      lcd.print(w,1);
    }
    if (w < targetForce + 0.2){
      myStepper.step(1);
      lcd.setCursor(0,2);
      lcd.print("CF: ");
      lcd.setCursor(4,2);
      lcd.print(w,1);
    }
    while (x < 250 && w > 0){
      homing();
    }
 }  

void homing() {

  c = scale.get_units();
  w = (c * 2.20462);
  lcd.setCursor(0,0);
  lcd.print("CF: ");
  lcd.setCursor(4,0);
  lcd.print(w,1);
  lcd.setCursor(0,2);
  lcd.print("      ");
  lcd.setCursor(5,2);
  lcd.print("Releasing");
  lcd.setCursor(15,2);
  lcd.print("    ");
  lcd.setCursor(4,3);
  lcd.print("Spring Force");

  while (digitalRead(limitPin) == HIGH) {
    myStepper.step(-1000);
    c = scale.get_units();
    w = (c * 2.20462);
    lcd.setCursor(0,0);
    lcd.print("CF: ");
    lcd.setCursor(4,0);
    lcd.print(w,1);
    lcd.setCursor(0,2);
  }

  homingComplete = true;
  
  analogWrite(ComPin, 255);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Test Completed");
  lcd.setCursor(1,2);
  lcd.print("Press Reset Button");

  
  //delay(5000);
  //lcd.clear();
  abort();
  
}

  





  

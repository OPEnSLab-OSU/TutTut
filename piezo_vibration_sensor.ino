/******************************************************************************
Tut-Tut_Piezo_Vibration_Sensor.ino
Jonathan Rohr
February 24, 2019

- Connect a 1Mohm resistor across the Piezo sensor's pins.
- Connect one leg of the Piezo to GND
- Connect the other leg of the piezo to A0

Vibrations on the Piezo sensor create voltage, which are sensed by the Arduino's
A0 pin. Check the serial monitor to view the voltage generated.
******************************************************************************/
#include <Adafruit_SleepyDog.h>


const int PIEZO_PIN = A0; // Piezo output
const int ledPIN = 13; // LED connected to digital pin 13

float sensorVals[5000];     // create sensor values storage array
float sensorAvgs[100];
float sensorNumDrops[100];
float sensorFreq[100];
float duration = 10;
int i = 0;
int avg = 0;
int n = 0;
int f = 0;
boolean loopReset = true;
unsigned long referenceTime = 0;
int ledState = LOW; // variable used to store the last LED status, to toggle the light
boolean raining = false;

void setup() 
{
  pinMode(ledPIN, OUTPUT); // declare the ledPin as the OUTPUT
  digitalWrite(ledPIN, HIGH); // Show device is awake
  Serial.begin(9600);
  attachInterrupt(PIEZO_PIN, wakeUpNow, LOW);
}

void loop() 
{
  if(raining == false){
    digitalWrite(ledPIN, LOW); // Show device is asleep
    attachInterrupt(PIEZO_PIN, wakeUpNow, RISING);
    int sleepMS = Watchdog.sleep();
    detachInterrupt(A0);
  }
  else if(raining == true){
    if(loopReset){
      referenceTime = millis()/1000;
    }
    if(call_timer(0, duration) == true){
      // Read Piezo ADC value in, and convert it to a voltage
      int piezoADC = analogRead(PIEZO_PIN);
      float piezoV = piezoADC / 1023.0 * 5.0;
      if(piezoV > .1) {
        //Serial.println(piezoV); // Print the voltage.
        sensorVals[i] = piezoV;
        i++;
        delay(100);
      }
      loopReset = false;
    }
    else if(call_timer(0, duration) == false){
      float temp = sensorVals[0];
      for(int j = 0; j < i; j++){
        temp = temp + sensorVals[j+1];
      }
      float average = temp / (i + 1);
      String strOne = "This is the average voltage for the last ";
      String strDos = strOne + (i+1);
      String strSan = strDos + " drops: ";
      String str4 = strSan + average;
      //Serial.println(str4);
      sensorAvgs[avg] = average;
      sensorNumDrops[n] = (i+1);
      sensorFreq[f] = ((i+1)/duration);
      f++;
      n++;
      avg++;
      i = 0;
      loopReset = true;
    }
    if(avg == 10){
      for(int a = 0; a < 10; a++){
        Serial.println(sensorAvgs[a]);
      }
      Serial.println("----------------------");
      for(int b = 0; b < 10; b++){
        Serial.println(sensorNumDrops[b]);
      }
      Serial.println("----------------------");
      for(int c = 0; c < 10; c++){
        Serial.println(sensorFreq[c]);
      }
      avg = 0;
    }
  }
}

boolean call_timer(int start, int duration){
  unsigned long timer = millis()/1000;
  unsigned long startTime = referenceTime + start;
  unsigned long endTime = startTime + duration;
  if(timer >= startTime && timer <= endTime){
    return true;
  }
  else{
    return false;
  }
}

void wakeUpNow(){
  digitalWrite(ledPIN, HIGH); // Show device is awake
  raining = true;
}

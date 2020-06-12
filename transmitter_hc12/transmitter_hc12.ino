/* 
    GND    ->   GND
    Vcc    ->   3.3V
    Tx     ->   D10
    Rx     ->   D11      
 */
#include <SoftwareSerial.h>

/* Works much better without the Serial communication to the Laptop */
#define SERIAL_TO_LAPTOP 0

/* Serial connection to the HC-12*/
SoftwareSerial HC12(10, 11);

/* Analog pin for the potentiometer */
int POTENTIOMETER = A2;

/* Old value of the Potentiometer */
int old_value_potentiometer;

void setup()
{
#if SERIAL_TO_LAPTOP == 1
  Serial.begin(9600); // Serial port to computer
#endif
  HC12.begin(9600); // Serial port to HC12
  pinMode(POTENTIOMETER, INPUT);
  old_value_potentiometer = map(analogRead(POTENTIOMETER), 0, 1024, 0, 255);
  HC12.write(old_value_potentiometer);
}

void loop()
{
  int val = map(analogRead(POTENTIOMETER), 0, 1024, 0, 255);
  if(old_value_potentiometer != val)
  {
    HC12.write(val);
    old_value_potentiometer = val;
  }
#if SERIAL_TO_LAPTOP == 1
  Serial.print("val : ");
  Serial.println(val);
#endif
  delay(50);
}

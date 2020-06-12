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

/* Analog pin for the LED */
int LED = 3;

void setup()
{
  HC12.begin(9600); // Serial port to HC12
  pinMode(LED, OUTPUT);

#if SERIAL_TO_LAPTOP == 1
  Serial.begin(9600); // Serial port to computer
  Serial.println("Receiver : ready to receive...");
#endif
}

void loop()
{
  while (HC12.available())
  { 
    int val = HC12.read();
    analogWrite(LED, val);
#if SERIAL_TO_LAPTOP == 1
    Serial.println(HC12.read()); // Send the data to Serial monitor
#endif
  }
}

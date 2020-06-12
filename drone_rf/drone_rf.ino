/*
    HC-12 Module : 
    GND    ->   GND
    Vcc    ->   3.3V
    Tx     ->   D10
    Rx     ->   D11      

    Pin M1 :
    D3
    
    Pin M2 :
    D5  
    
    Pin M3 :
    D6  
    
    Pin M4 :
    D9
 */

#include <SoftwareSerial.h>
#include <Servo.h>

/* Works much better without the Serial communication to the Laptop */
#define SERIAL_TO_LAPTOP 1

/* Struct for a message 
* [0] : 0x02 (STX)
* [1] : value_speed
* [2] : value_lr
* [3] : value_ud
* [4] : 0x03 (ETX)
*/
struct commands_message
{
  const unsigned char STX = 0x02;
  unsigned char speed;
  unsigned char lr;
  unsigned char ud;
  const unsigned char ETX = 0x03;
};
#define MSG_SIZE 5

/* Serial connection to the HC-12*/
SoftwareSerial HC12(10, 11);

/* Analog pin for the LED */
const int PIN_MOTOR_1 = 3;
const int PIN_MOTOR_2 = 5;
const int PIN_MOTOR_3 = 6;
const int PIN_MOTOR_4 = 9;

/* Consigns */
int m1, m2, m3, m4;

/* Setup the PWMs output */
void setup_pwm_outputs()
{
  pinMode(PIN_MOTOR_1, OUTPUT);
  pinMode(PIN_MOTOR_2, OUTPUT);
  pinMode(PIN_MOTOR_3, OUTPUT);
  pinMode(PIN_MOTOR_4, OUTPUT);
}

/* Update PWMs */
void update_actuators(int m1, int m2, int m3, int m4)
{
  analogWrite(PIN_MOTOR_1, m1);
  analogWrite(PIN_MOTOR_2, m2);
  analogWrite(PIN_MOTOR_3, m3);
  analogWrite(PIN_MOTOR_4, m4);
}

/* Print a command_message */
void display_msg(commands_message* msg)
{
#if SERIAL_TO_LAPTOP == 1
  Serial.println("*** Command message ***");
  Serial.print("speed : ");
  Serial.println(msg->speed);
  Serial.print("lr : ");
  Serial.println(msg->lr);
  Serial.print("ud : ");
  Serial.println(msg->ud);
#endif
}

/* Read a message from the HC-12 */
bool read_command_message(commands_message *msg)
{
  /* not blocking */
  if (HC12.available() <= MSG_SIZE)
  {
    return false;
  }
  /* Wait for the beginning of a commands_message, must be STX */
  while (HC12.read() != 0x02)
  {
  }
  (*msg).speed = HC12.read();
  (*msg).lr = HC12.read();
  (*msg).ud = HC12.read();

  /* Check for the end of the commands_message, must be ETX */
  if (HC12.read() != 0x03)
  {
#if SERIAL_TO_LAPTOP == 1
    Serial.println("BAD MESSAGE RECEIVED");
#endif
    return false;
  }

  /* Display the received command_message */
  //display_msg(msg);
  return true;
}

void setup()
{
  /* Serial connection to the HC-12 */
  HC12.begin(9600);

  /* PWMs set up */
  setup_pwm_outputs();

  /* Init consigns */
  m1 = 0;
  m2 = 0;
  m3 = 0;
  m4 = 0;

#if SERIAL_TO_LAPTOP == 1
  Serial.begin(9600); // Serial port to computer
  Serial.println("Drone ready to take off !");
#endif
}

void loop()
{
  /* Read data from the HC-12 */
  commands_message msg;
  bool msg_is_valid = read_command_message(&msg);

  /* Should the message be used or not ? */
  int speed, lr, ud = 0;
  if (msg_is_valid)
  {
    speed  = msg.speed;
    lr = msg.lr;
    ud = msg.ud;
  }

  // process speed command
  if (speed > 0)
  {
    m1 = 255;
    m3 = 255;
    m2 = 100;
    m4 = 100;
  }
  else if (speed < 0)
  {
    m1 = 100;
    m3 = 100;
    m2 = 255;
    m4 = 255;
  }
  else
  {
    m1 = m1;
    m2 = m2;
    m3 = m3;
    m4 = m4;
  }

  // process up/down command
  if (ud > 0)
  {
    m1 = 255;
    m3 = 255;
    m2 = 255;
    m4 = 255;
  }
  else if (ud < 0)
  {
    m1 = 100;
    m3 = 100;
    m2 = 100;
    m4 = 100;
  }
  else
  {
    m1 = m1;
    m2 = m2;
    m3 = m3;
    m4 = m4;
  }

  // process direction command
  if (lr > 0)
  {
    m1 = 255;
    m2 = 255;
    m3 = 100;
    m4 = 100;
  }
  else if (lr < 0)
  {
    m1 = 100;
    m2 = 100;
    m3 = 255;
    m4 = 255;
  }
  else
  {
    m1 = m1;
    m2 = m2;
    m3 = m3;
    m4 = m4;
  }
  
  

  /* Check for the gyroscope and update values of the cmd if needed */

  /* Check for the altimeter and update values of the cmd if needed */

  /* Performs actions on the actuators */
  update_actuators(m1, m2, m3, m4);

  delay(80);
}

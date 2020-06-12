/* 
    HC-12 Module : 
    GND    ->   GND
    Vcc    ->   3.3V
    Tx     ->   D10
    Rx     ->   D11  

    Speed Potentiometer : 
    A2

    LR Joystick(X) : 
    A3

    UD Joystick(Y) :
    A4    
 */

#include <SoftwareSerial.h>

/* Works much better without the Serial communication to the Laptop */
#define SERIAL_TO_LAPTOP 1

/* Range for potentiometers */
#define MAX_POT 1023
#define MAX_PWM 255
#define MIN_JOYSTICK 256
#define MAX_JOYSTICK 767
#define MAX_SERVO 180

/* Serial connection to the HC-12*/
SoftwareSerial HC12(10, 11);

/* Analog pin for the potentiometer */
const int SPEED_POTENTIOMETER = A2;
const int LR_JOYSTICK = A3;
const int UD_JOYSTICK = A4;

/* Old value of the commands */
int old_speed;
int old_lr;
int old_ud;

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

/* Setup the analog inputs */
void setup_analog_inputs()
{
  pinMode(SPEED_POTENTIOMETER, INPUT);
  pinMode(LR_JOYSTICK, INPUT);
  pinMode(UD_JOYSTICK, INPUT);
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

/* Send a command_message to the receiver */
void send_command_message(commands_message *msg)
{
  HC12.write(msg->STX);
  HC12.write(msg->speed);
  HC12.write(msg->lr);
  HC12.write(msg->ud);
  HC12.write(msg->ETX);

  /* Display the message on the console */
  display_msg(msg);
}

/* Read all the analog values */
void read_inputs(int *speed, int *lr, int *ud)
{
  (*speed) = map(analogRead(SPEED_POTENTIOMETER), 0, MAX_POT, 0, MAX_PWM);
  (*lr) = map(analogRead(LR_JOYSTICK), MIN_JOYSTICK, MAX_JOYSTICK, 0, MAX_SERVO);
  (*ud) = map(analogRead(UD_JOYSTICK), MIN_JOYSTICK, MAX_JOYSTICK, 0, MAX_SERVO);
}

void setup()
{
#if SERIAL_TO_LAPTOP == 1
  Serial.begin(9600); // Serial port to computer
#endif

  /* Serial connection to the HC-12 */
  HC12.begin(9600);

  /* Analog read setup */
  setup_analog_inputs();

  /* Get the values at startup */
  read_inputs(&old_speed, &old_lr, &old_ud);

  /* Send the start up message */
  commands_message initial_message;
  initial_message.speed = old_speed;
  initial_message.lr = old_lr;
  initial_message.ud = old_ud;
  send_command_message(&initial_message);
}

void loop()
{
  /* Read the inputs */
  int speed, lr, ud;
  read_inputs(&speed, &lr, &ud);

  /* Check for any change */
  bool cmd_has_to_be_sent = false;
  if (old_speed != speed || old_lr != lr || old_ud != ud)
  {
    cmd_has_to_be_sent = true;
  }

  /* Send a new command_message if the commands changed */
  if (cmd_has_to_be_sent)
  {
    commands_message message;
    message.speed = speed;
    message.lr = lr;
    message.ud = ud;
    send_command_message(&message);

    /* Now memorize the old values */
    old_speed = speed;
    old_lr = lr;
    old_ud = ud;
  }

  /* Sleep for a while */
  delay(100);
}

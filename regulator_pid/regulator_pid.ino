// Using an Arduino Nano 33 BLE Sense (0) or and Arduino Nano 33 IoT (1)?
#define ARDUINO_NANO 1

// Include libraries for the sensors
#if ARDUINO_NAN0 == 0
#include <Arduino_LSM9DS1.h> /* 9 axis IMU for Arduino Nano 33 BLE Sense*/
#elif ARDUINO_NAN0 == 1
#include <Arduino_LSM6DS3.h> /* 9 axis IMU for Arduino Nano 33 IoT*/
#endif

// working with maths
#include <math.h>

// conversion macros
#define RAD_2_DEGREE(angle) (angle * 180.0 / M_PI)
#define DEGREE_2_RAD(angle) (angle * M_PI / 180.0)
#define ROUND_N_DIGITS(value, dozen) ((int32_t(value * pow(10.0f, dozen))) / pow(10.0f, dozen))

// PI coefficients
const float PROP_COEFF = 0.3f;
const float DER_COEFF = 0.12f;
const uint8_t PWM_BASE = 100;

// memorized var
static float stored_pitch = 0.0f;
static float stored_roll = 0.0f;
static uint64_t stored_time = 0;

// basic consigns
static float consign_pitch = 0.0f;
static float consign_roll = 0.0f;

// motor side
enum MOTOR_SIDE
{
  MOTOR_POS = 0, 
  MOTOR_NEG = 1
};

// motor pins
const int PIN_M1 = 3;
const int PIN_M2 = 5;
const int PIN_M3 = 6;
const int PIN_M4 = 9;

uint8_t get_pwm_consign(float delta, float derivative, MOTOR_SIDE side)
{
  float offset = PROP_COEFF * delta + DER_COEFF * delta;
  float consign = PWM_BASE;
  switch(side)
  {
    // motor that makes turning toward negative direction
    case MOTOR_POS:
      consign -= offset;
      break;
      
    // motor that makes turning toward negative direction
    case MOTOR_NEG:
      consign += offset;
      break;
  }
  if (consign <= 50.0f) {return 0x32;}
  else if (consign >= 255.0f) {return 0xff;}
  else {return (0xff & uint8_t(consign));}
}

void setup() {
  // Serial communication with PC
  Serial.begin(115200);

  // Initialize the 9 axis IMU
  if (!IMU.begin()) 
  {
    Serial.println("Failed to initialize IMU...");
    while (1){Serial.println("IMU initialialization failed..");}
  }
  
  // Initialize stored values
  float x, y, z;
  if (IMU.accelerationAvailable()) 
  {
    stored_time = millis();
    IMU.readAcceleration(x, y, z);
    stored_pitch = RAD_2_DEGREE(asin(x));
    stored_roll = RAD_2_DEGREE(asin(y));
  }

  // init pins
  pinMode(PIN_M1, OUTPUT);
  pinMode(PIN_M2, OUTPUT);
  pinMode(PIN_M3, OUTPUT);
  pinMode(PIN_M4, OUTPUT);
}

void loop() 
{
  float x, y, z;
  if (IMU.accelerationAvailable()) 
  {
    // Get the latest pitch and roll
    unsigned long time = millis();
    IMU.readAcceleration(x, y, z);
    /*
    float pitch = RAD_2_DEGREE(asin(ROUND_N_DIGITS(x, 2)));
    float roll = RAD_2_DEGREE(asin(ROUND_N_DIGITS(y, 2)));
    */
    float pitch = RAD_2_DEGREE(asin(x));
    float roll = RAD_2_DEGREE(asin(y));

    // compute delta and derivatives
    unsigned long delta_time = time - stored_time;
    float derivative_pitch = ROUND_N_DIGITS((pitch - stored_pitch) / delta_time * 1000, 1); // degree/second
    float derivative_roll = ROUND_N_DIGITS((roll - stored_roll) / delta_time * 1000, 1); // degree/second
    float delta_pitch = consign_pitch - pitch;
    float delta_roll = consign_roll - roll;
    
    /*
    Serial.print("Pitch [DEG]: ");
    Serial.print(pitch);
    Serial.print(", dp/dt [DEG/S] : ");
    Serial.print(derivative_pitch);
    Serial.print("\tRoll : ");
    Serial.print(roll);
    Serial.print(", dr/dt [DEG/S] : ");
    Serial.println(derivative_roll);
    */
    float pwm_1 = get_pwm_consign(delta_pitch, derivative_pitch, MOTOR_POS);
    analogWrite(PIN_M1, pwm_1);
    float pwm_2 = get_pwm_consign(delta_pitch, derivative_pitch, MOTOR_NEG);
    analogWrite(PIN_M2, pwm_2);
    Serial.print("PWM left : ");
    Serial.print(pwm_1);
    Serial.print(", PWM right : ");
    Serial.println(pwm_2);
    
    // save pitch, roll and time
    stored_pitch = pitch;
    stored_roll = roll;
    stored_time = time;
    
    // wait some time
    delay(180);
  }
}

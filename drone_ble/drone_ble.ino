// Using an Arduino Nano 33 BLE Sense (0) or and Arduino Nano IoT (1)?
#define ARDUINO_NANO 1

// Include libraries for the sensors
/*
#if ARDUINO_NAN0 == 0
#include <Arduino_LSM9DS1.h> // 9 axis IMU
#elif ARDUINO_NAN0 == 1
#include <Arduino_LSM6DS3.h> // 9 axis IMU
#endif
*/
#include <Arduino_LSM6DS3.h> // 9 axis IMU

// standard includes
#include <ArduinoBLE.h>
#include <math.h>

// conversion macros
#define RAD_2_DEGREE(angle) (angle * 180.0 / M_PI)
#define DEGREE_2_RAD(angle) (angle * M_PI / 180.0)
#define ROUND_N_DIGITS(value, dozen) ((int32_t(value * pow(10.0f, dozen))) / pow(10.0f, dozen))

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

// PI coefficients
const float P_COEFF = 0.3f;
const float I_COEFF = 0.1f;
const float D_COEFF = 0.1f;

// memorized var
static float stored_pitch = 0.0f;
static float stored_roll = 0.0f;
static uint64_t stored_time = 0;

// basic consigns
static float consign_pitch = 0.0f;
static float consign_roll = 0.0f;
static uint8_t speed = 0;

// BLE service and characteristics
BLEService drone_service("2700");
BLEFloatCharacteristic pitch_characteristic("2701", BLERead | BLEWrite);
BLEFloatCharacteristic roll_characteristic("2702", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic speed_characteristic("2703", BLERead | BLEWrite);

uint8_t get_pwm_consign(float delta, float derivative, float integrale, MOTOR_SIDE side)
{
    float offset = P_COEFF * delta + D_COEFF * derivative + I_COEFF * integrale;
    float consign = speed;
    if (consign < 20.0)
    {
        return 0x00;
    }
    switch (side)
    {
    // motor that makes turning toward positive direction
    case MOTOR_POS:
        consign -= offset;
        break;

    // motor that makes turning toward negative direction
    case MOTOR_NEG:
        consign += offset;
        break;
    }
    if (consign >= 255.0f)
    {
        return 0xff;
    }
    else
    {
        return (0xff & uint8_t(consign));
    }
}

void setup()
{
    // Serial communication set up at 115200 bauds
    Serial.begin(115200);

    // Initialize BLE
    if (!BLE.begin())
    {
        while (1)
        {
            Serial.println("BLE initialization failed...");
        }
    }

    // Initialize the 9 axis IMU
    if (!IMU.begin())
    {
        while (1)
        {
            Serial.println("IMU initialialization failed..");
        }
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

    // Initialize pins
    pinMode(PIN_M1, OUTPUT);
    pinMode(PIN_M2, OUTPUT);
    pinMode(PIN_M3, OUTPUT);
    pinMode(PIN_M4, OUTPUT);

    // Prepare BLE peripheral
    BLE.setLocalName("Drone33");
    BLE.setAdvertisedService(drone_service);
    drone_service.addCharacteristic(pitch_characteristic);
    drone_service.addCharacteristic(roll_characteristic);
    drone_service.addCharacteristic(speed_characteristic);
    BLE.addService(drone_service);
    /*
    BLE.setEventHandler(BLEConnected, connected_cb);
    BLE.setEventHandler(BLEDisconnected, disconnected_cb);
    */
    pitch_characteristic.setEventHandler(BLEWritten, pitch_written_cb);
    roll_characteristic.setEventHandler(BLEWritten, roll_written_cb);
    speed_characteristic.setEventHandler(BLEWritten, speed_written_cb);
    pitch_characteristic.writeValue(0.0f);
    roll_characteristic.writeValue(0.0f);
    speed_characteristic.writeValue(0x00);

    // Start advertising
    BLE.advertise();
    Serial.println("Bluetooth device active, waiting for connections...");
}

void loop()
{
    // Get latest BLE events
    BLE.poll();

    // process the drone
    float x, y, z;
    if (IMU.accelerationAvailable())
    {
        // Get the latest pitch and roll
        unsigned long time = millis();
        IMU.readAcceleration(x, y, z);
        float pitch = RAD_2_DEGREE(asin(x));
        float roll = RAD_2_DEGREE(asin(y));

        // compute delta and derivatives
        unsigned long delta_time = time - stored_time;
        float delta_pitch = ROUND_N_DIGITS(pitch - consign_pitch, 1);
        float delta_roll = ROUND_N_DIGITS(roll - consign_roll, 1);
        float derivative_pitch = ROUND_N_DIGITS((pitch - stored_pitch) / delta_time * 1000, 1); // degree/second
        float derivative_roll = ROUND_N_DIGITS((roll - stored_roll) / delta_time * 1000, 1);    // degree/second
        //float integrale_pitch = ROUND_N_DIGITS(, 1)

        // compute PWM consigns
        float pwm_1 = get_pwm_consign(delta_pitch, 0.0f ,0.0f,  MOTOR_POS);
        analogWrite(PIN_M1, pwm_1);
        float pwm_2 = get_pwm_consign(delta_pitch, 0.0f, 0.0f, MOTOR_NEG);
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
        delay(100);
    }
}

/*
void connected_cb(BLECentral &central)
{
    Serial.print("Connected event, central: ");
    Serial.println(central.address());
}

void disconnected_cb(BLECentral &central)
{
    Serial.print("Disconnected event, central: ");
    Serial.println(central.address());
}
*/
void pitch_written_cb(BLEDevice central, BLECharacteristic characteristic)
{
    const float new_pitch = pitch_characteristic.value();
    consign_pitch = new_pitch;
}

void roll_written_cb(BLEDevice central, BLECharacteristic characteristic)
{
    const float new_roll = roll_characteristic.value();
    consign_roll = new_roll;
}

void speed_written_cb(BLEDevice central, BLECharacteristic characteristic)
{
    const unsigned char new_speed = speed_characteristic.value();
    speed = new_speed;
}

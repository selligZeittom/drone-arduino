#include <ArduinoBLE.h>

// analog sensors
#define MIN_POT 0
#define MAX_POT 1023
#define MIN_JOYSTICK 256
#define MAX_JOYSTICK 767

// mapped limits
#define MIN_ANGLE (-20.0f)
#define MAX_ANGLE (20.0f)
#define MIN_SPEED 0
#define MAX_SPEED 200

// pins
const uint8_t PIN_PITCH_JOYSTICK = A0;
const uint8_t PIN_ROLL_JOYSTICK = A1;
const uint8_t PIN_SPEED = A2;

void setup() {

  // Serial communication with PC
  Serial.begin(115200);

  // initialize BLE
  if (!BLE.begin()) 
  {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // intialize pins
  pinMode(PIN_PITCH_JOYSTICK, INPUT);
  pinMode(PIN_ROLL_JOYSTICK, INPUT);
  pinMode(PIN_SPEED, INPUT);
  delay(3000);

  // find the peripheral
  BLE.scanForUuid("2700");
}

void sensors(BLEDevice peripheral)
{
  // connect to the peripheral
  if(peripheral.connect())
  {
    Serial.println("Connected!");
  } else {
    Serial.println("Failed to connect");
    return;
  }

  // discover attributes
  if(!peripheral.discoverAttributes())
  {
    Serial.println("Failed to discover attributes");
    return;
  }

  // get the characteristics
  BLECharacteristic pitch_characteristic =  peripheral.characteristic("2701");
  if((!pitch_characteristic) || (!pitch_characteristic.canWrite()))
  {
    Serial.println("Pitch characteristic is either unavailable or not writtable");
    peripheral.disconnect();
    return;
  }
  BLECharacteristic roll_characteristic =  peripheral.characteristic("2702");
  if((!roll_characteristic) || (!roll_characteristic.canWrite()))
  {
    Serial.println("Roll characteristic is either unavailable or not writtable");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic speed_characteristic =  peripheral.characteristic("2703");
  if((!speed_characteristic) || (!speed_characteristic.canWrite()))
  {
    Serial.println("Speed characteristic is either unavailable or not writtable");
    peripheral.disconnect();
    return;
  }

  while(peripheral.connected())
  {
    // pitch
    int32_t pitch_joystick = analogRead(PIN_PITCH_JOYSTICK);
    float pitch = map(pitch_joystick, MIN_JOYSTICK, MAX_JOYSTICK, MIN_ANGLE, MAX_ANGLE);
    byte pitch_bytes[4] = {0};
    memcpy(pitch_bytes, (unsigned char*) (&pitch), 4);

    // roll
    int32_t roll_joystick = analogRead(PIN_ROLL_JOYSTICK);
    float roll = map(roll_joystick, MIN_JOYSTICK, MAX_JOYSTICK, MIN_ANGLE, MAX_ANGLE);
    byte roll_bytes[4] = {0};
    memcpy(roll_bytes, (unsigned char*) (&roll), 4);
    
    // speed
    int32_t speed_pot = analogRead(PIN_SPEED);
    uint8_t speed = map(speed_pot, MIN_POT, MAX_POT, MIN_SPEED, MAX_SPEED);

    // print results
    char buffer[50];
    sprintf(buffer, "Pitch : %f, roll : %f, speed : %d\0", pitch, roll, speed);
    Serial.println(buffer);

    // write results
    pitch_characteristic.writeValue(pitch_bytes, 4);
    roll_characteristic.writeValue(roll_bytes, 4);
    speed_characteristic.writeValue(speed);
    delay(50);
  }

  // peripheral has disconnected
  Serial.println("Peripheral disconnected...");
  
  /*
  Serial.print("pitch : ");
  Serial.print(pitch);
  Serial.print(", roll : ");
  Serial.print(roll);
  Serial.print(", speed : ");
  Serial.println(speed);
  */
}

void loop() {
  BLEDevice peripheral = BLE.available();
  if(peripheral){
    Serial.println("Found the drone.");

    // stop scanning
    BLE.stopScan();

    // process sensors data
    sensors(peripheral);

    // find the peripheral again if it disconnected
    BLE.scanForUuid("2700");
  }
  else
  {
    Serial.println("Drone not found yet...");
  }
  delay(100); 
}

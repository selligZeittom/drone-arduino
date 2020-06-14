/* Include libraries for the sensors */
#include <Arduino_LSM9DS1.h> /* 9 axis IMU */

/* Include library for BLE */
#include <ArduinoBLE.h>

/* motor PIN */
const int PIN_M1 = 3;
const int PIN_M2 = 5;
const int PIN_M3 = 6;
const int PIN_M4 = 9;

/* variables for the BLE exchanges */
BLEService drone_service("2700");
BLEFloatCharacteristic pitch_characteristic("2701", BLERead | BLEWrite);
BLEFloatCharacteristic roll_characteristic("2702", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic speed_characteristic("2703", BLERead | BLEWrite);

void setup()
{
    /* Serial communication set up at 115200 bauds */
    Serial.begin(9600);

    /* Initialize BLE */
    if (!BLE.begin())
    {
        Serial.println("Failed to initialize BLE...");
        while (1);
    }

    /* Initialize pins */
    pinMode(PIN_M1, OUTPUT);
    pinMode(PIN_M2, OUTPUT);
    pinMode(PIN_M3, OUTPUT);
    pinMode(PIN_M4, OUTPUT);

    /* Set the name of the device and the service */
    BLE.setLocalName("Drone33");
    BLE.setAdvertisedService(drone_service);

    /* Add the characteristics to the device */
    drone_service.addCharacteristic(pitch_characteristic);
    drone_service.addCharacteristic(roll_characteristic);
    drone_service.addCharacteristic(speed_characteristic);
    
    /* Add service to peripheral */
    BLE.addService(drone_service);

    /* callback for connection/disconnection events*/
    /*
    BLE.setEventHandler(BLEConnected, connected_cb);
    BLE.setEventHandler(BLEDisconnected, disconnected_cb);
    */

    /* callback for write events */
    pitch_characteristic.setEventHandler(BLEWritten, pitch_written_cb);
    roll_characteristic.setEventHandler(BLEWritten, roll_written_cb);
    speed_characteristic.setEventHandler(BLEWritten, speed_written_cb);

    /* Set an initial value */
    pitch_characteristic.writeValue(0.0f);
    roll_characteristic.writeValue(0.0f);
    speed_characteristic.writeValue(0x00);

    /* Start advertising */
    BLE.advertise();
    Serial.println("Bluetooth device active, waiting for connections...");
}

void loop()
{
    BLE.poll();
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
    Serial.print("Pitch : ");
    Serial.println(new_pitch);
}

void roll_written_cb(BLEDevice central, BLECharacteristic characteristic)
{
    const float new_roll = roll_characteristic.value();
    Serial.print("Roll : ");
    Serial.println(new_roll);
}

void speed_written_cb(BLEDevice central, BLECharacteristic characteristic)
{
    const unsigned char new_speed = speed_characteristic.value();
    Serial.print("Speed : ");
    Serial.println(new_speed);
}

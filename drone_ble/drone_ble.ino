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
BLEService drone_service("19B10010-E8F2-537E-4F6C-D104768A1214");
BLEUnsignedCharCharacteristic drone_m1_characteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic drone_m2_characteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic drone_m3_characteristic("19B10013-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic drone_m4_characteristic("19B10014-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

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
    drone_service.addCharacteristic(drone_m1_characteristic);
    drone_service.addCharacteristic(drone_m2_characteristic);
    drone_service.addCharacteristic(drone_m3_characteristic);
    drone_service.addCharacteristic(drone_m4_characteristic);
    
    /* Add service to peripheral */
    BLE.addService(drone_service);

    /* callback for connection/disconnection events*/
    /*
    BLE.setEventHandler(BLEConnected, connected_cb);
    BLE.setEventHandler(BLEDisconnected, disconnected_cb);
    */

    /* callback for write events */
    drone_m1_characteristic.setEventHandler(BLEWritten, m1_cb);
    drone_m2_characteristic.setEventHandler(BLEWritten, m2_cb);
    drone_m3_characteristic.setEventHandler(BLEWritten, m3_cb);
    drone_m4_characteristic.setEventHandler(BLEWritten, m4_cb);

    /* Set an initial value */
    drone_m1_characteristic.writeValue(0);
    drone_m2_characteristic.writeValue(0);
    drone_m3_characteristic.writeValue(0);
    drone_m4_characteristic.writeValue(0);

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
void m1_cb(BLEDevice central, BLECharacteristic characteristic)
{
    Serial.print("M1 : ");
    const unsigned char m1 = drone_m1_characteristic.value();
    Serial.println(m1);
    analogWrite(PIN_M1, m1);
}

void m2_cb(BLEDevice central, BLECharacteristic characteristic)
{
    Serial.print("M2 : ");
    const unsigned char m2 = drone_m1_characteristic.value();
    Serial.println(m2);
    analogWrite(PIN_M2, m2);
}

void m3_cb(BLEDevice central, BLECharacteristic characteristic)
{
    Serial.print("M3 : ");
    const unsigned char m3 = drone_m1_characteristic.value();
    Serial.println(m3);
    analogWrite(PIN_M3, m3);
}

void m4_cb(BLEDevice central, BLECharacteristic characteristic)
{
    Serial.print("M4 : ");
    const unsigned char m4 = drone_m1_characteristic.value();
    Serial.println(m4);
    analogWrite(PIN_M4, m4);
}
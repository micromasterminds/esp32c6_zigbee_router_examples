
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://

// SCD30 config
#define SCD30_SDA_PIN 6
#define SCD30_SCL_PIN 7

/* Zigbee carbon dioxide sensor configuration */
#define CARBON_DIOXIDE_SENSOR_ENDPOINT_NUMBER 10
#define TEMPERATURE_SENSOR_ENDPOINT_NUMBER 11
uint8_t button = BOOT_PIN;

ZigbeeCarbonDioxideSensor zbCarbonDioxideSensor = ZigbeeCarbonDioxideSensor(CARBON_DIOXIDE_SENSOR_ENDPOINT_NUMBER);
ZigbeeTempSensor zbTempSensor = ZigbeeTempSensor(TEMPERATURE_SENSOR_ENDPOINT_NUMBER);
SCD30 airSensor;


void setup() {
  Serial.begin(115200);
  Wire.begin(SCD30_SDA_PIN, SCD30_SCL_PIN);  // initialize I2C protocoll

  // Init button switch
  pinMode(button, INPUT_PULLUP);

  delay(2000);

  // Checking button for factory reset and reporting
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
  }


  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    //while (1)
    //  ;
  }

  airSensor.setAutoSelfCalibration(true);
  log_i("Auto calibration set to %d", airSensor.getAutoSelfCalibration());

  // Optional: set Zigbee device name and model
  zbCarbonDioxideSensor.setManufacturerAndModel("MicroMasterMinds", "CarbonDioxideSensor");
  zbTempSensor.setManufacturerAndModel("MicroMasterMinds", "TemperatureSensor");

  // Set minimum and maximum carbon dioxide measurement value in ppm
  zbCarbonDioxideSensor.setMinMaxValue(400, 10000);
  zbTempSensor.setMinMaxValue(-40, 70);
  zbTempSensor.addHumiditySensor(0, 95, 3);

  // Add endpoints to Zigbee Core
  Zigbee.addEndpoint(&zbCarbonDioxideSensor);
  Zigbee.addEndpoint(&zbTempSensor);

  Serial.println("Starting Zigbee...");
  // When all EPs are registered, start Zigbee in End Device mode
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Zigbee started successfully!");
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Set reporting interval for carbon dioxide measurement to be done every 30 seconds, must be called after Zigbee.begin()
  // min_interval and max_interval in seconds, delta (carbon dioxide change in ppm)
  // if min = 1 and max = 0, reporting is sent only when carbon dioxide changes by delta
  // if min = 0 and max = 10, reporting is sent every 10 seconds or when carbon dioxide changes by delta
  // if min = 0, max = 10 and delta = 0, reporting is sent every 10 seconds regardless of delta change
  zbCarbonDioxideSensor.setReporting(0, 30, 0);
  zbTempSensor.setReporting(0, 30, 0);
}

void loop() {
  static uint32_t timeCounter = 0;
  // Read carbon dioxide sensor every 2s
  if (!(timeCounter++ % 20)) {  // delaying for 100ms x 20 = 2s
    // Read sensor value - here is chip temperature used + 300 as a dummy value for demonstration
    uint16_t carbon_dioxide_value = airSensor.getCO2();
    float temp_value = airSensor.getTemperature();
    float humidity_value = airSensor.getHumidity();
    //Serial.printf("Updating carbon dioxide sensor value to %d ppm\r\n", carbon_dioxide_value);
    zbCarbonDioxideSensor.setCarbonDioxide(carbon_dioxide_value);
    zbTempSensor.setTemperature(temp_value);
    zbTempSensor.setHumidity(humidity_value);
  }

  // Checking button for factory reset and reporting
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    zbCarbonDioxideSensor.report();
    zbTempSensor.report();
  }
  delay(100);
}

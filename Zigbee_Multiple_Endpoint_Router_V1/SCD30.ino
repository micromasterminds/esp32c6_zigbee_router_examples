#include <Wire.h>
#include <SparkFun_SCD30_Arduino_Library.h>

#define SCD30_SDA_PIN 6
#define SCD30_SCL_PIN 7

SCD30 airSensor;

static void SCD30_sensor_handler(void *pvParameters) {

  Wire.begin(SCD30_SDA_PIN, SCD30_SCL_PIN);  // initialize I2C protocoll
  vTaskDelay(10 / portTICK_PERIOD_MS);       // delay alternative for Tasks. Dont use delay()!

  while (airSensor.begin(Wire, true) == false) {
    log_e("Air sensor not detected. Please check wiring....");
  }

  airSensor.setAutoSelfCalibration(true);
  log_i("Auto calibration set to %d", airSensor.getAutoSelfCalibration());

  while (true) {
    if (airSensor.dataAvailable()) {
      SCD30_CO2 = airSensor.getCO2();
      SCD30_temp = airSensor.getTemperature();
      SCD30_humid = airSensor.getHumidity();
      Serial.print("SCD30           ");
      Serial.print("Temperature: ");
      Serial.print(SCD30_temp);
      Serial.print(" C    ");
      Serial.print("Humidity:    ");
      Serial.print(SCD30_humid);
      Serial.print(" %    ");
      Serial.print("CO2:         ");
      Serial.print(SCD30_CO2);
      Serial.println(" ppm    ");
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);  // delay alternative for Tasks. Dont use delay()!
  }
}
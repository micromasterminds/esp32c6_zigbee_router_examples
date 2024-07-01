// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Based on Espressif Examples
//
// Modified by MicroMasterMinds 🤯
//
// Hoffe es funzt auch 😂

#ifndef ZIGBEE_MODE_ZCZR  // Coordinator Device or Router Device
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "esp_zigbee_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ha/esp_zigbee_ha_standard.h"
#include "defines.h"

int SCD30_CO2;
float SCD30_temp, SCD30_humid;

/********************* Zigbee functions **************************/
static void esp_zb_task(void *pvParameters) {
  /* Initialize Zigbee platform */
  esp_zb_platform_config_t platformConfig = {};
  platformConfig.radio_config.radio_mode = ZB_RADIO_MODE_NATIVE;
  platformConfig.host_config.host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE;
  ESP_ERROR_CHECK(esp_zb_platform_config(&platformConfig));

  /* Initialize Zigbee stack */
  esp_zb_cfg_t stackConfig = {};
  stackConfig.esp_zb_role = ESP_ZB_DEVICE_TYPE_ROUTER;
  stackConfig.install_code_policy = INSTALLCODE_POLICY_ENABLE;
  stackConfig.nwk_cfg.zczr_cfg.max_children = MAX_CHILDREN;
  esp_zb_init(&stackConfig);

  /* Register the device */
  esp_zb_ep_list_t *endpointList = esp_zb_ep_list_create();
  createZigbeeTempSensorDevice(endpointList);     /* create temperature endpoint and add it to endpoint list */
  createZigbeeCo2SensorDevice(endpointList);      /* create co2 endpoint and add it to endpoint list */
  createZigbeeLightDevice(endpointList);          /* create light endpoint and add it to endpoint list */
  createZigbeeHumiditySensorDevice(endpointList); /* create humidity endpoint and add it to endpoint list */
  esp_zb_device_register(endpointList);           /* take endpoint list and register the whole list */

  /* Config the reporting info  */
  esp_zb_zcl_reporting_info_t temp_reporting_info = {};
  temp_reporting_info.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
  temp_reporting_info.ep = HA_ESP_TEMPERATURE_ENDPOINT;  // Temperature sensor endpoint
  temp_reporting_info.cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT;
  temp_reporting_info.cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE;
  temp_reporting_info.dst.profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  temp_reporting_info.u.send_info.min_interval = 0;
  temp_reporting_info.u.send_info.max_interval = 1;
  temp_reporting_info.u.send_info.delta.u16 = 0;
  temp_reporting_info.u.send_info.def_min_interval = 0;
  temp_reporting_info.u.send_info.def_max_interval = 1;
  temp_reporting_info.attr_id = ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID;
  temp_reporting_info.manuf_code = ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC;
  ESP_ERROR_CHECK(esp_zb_zcl_update_reporting_info(&temp_reporting_info));
  log_i("UPDATE REPORTING INFO TEMP");

  /* Config the reporting info  */
  esp_zb_zcl_reporting_info_t co2_reporting_info = {};
  co2_reporting_info.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
  co2_reporting_info.ep = HA_ESP_CO2_ENDPOINT;
  co2_reporting_info.cluster_id = ESP_ZB_ZCL_CLUSTER_ID_CARBON_DIOXIDE_MEASUREMENT;
  co2_reporting_info.cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE;
  co2_reporting_info.dst.profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  co2_reporting_info.u.send_info.min_interval = 0;
  co2_reporting_info.u.send_info.max_interval = 1;
  co2_reporting_info.u.send_info.delta.u16 = 0;
  co2_reporting_info.u.send_info.def_min_interval = 0;
  co2_reporting_info.u.send_info.def_max_interval = 1;
  co2_reporting_info.attr_id = ESP_ZB_ZCL_ATTR_CARBON_DIOXIDE_MEASUREMENT_MEASURED_VALUE_ID;
  co2_reporting_info.manuf_code = ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC;
  ESP_ERROR_CHECK(esp_zb_zcl_update_reporting_info(&co2_reporting_info));
  log_i("UPDATE REPORTING INFO CO2");

  /* Config the reporting info  */
  esp_zb_zcl_reporting_info_t humidity_reporting_info = {};
  humidity_reporting_info.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
  humidity_reporting_info.ep = HA_ESP_HUMIDITY_ENDPOINT;
  humidity_reporting_info.cluster_id = ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT;
  humidity_reporting_info.cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE;
  humidity_reporting_info.dst.profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  humidity_reporting_info.u.send_info.min_interval = 0;
  humidity_reporting_info.u.send_info.max_interval = 1;
  humidity_reporting_info.u.send_info.delta.u16 = 0;
  humidity_reporting_info.u.send_info.def_min_interval = 0;
  humidity_reporting_info.u.send_info.def_max_interval = 1;
  humidity_reporting_info.attr_id = ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID;
  humidity_reporting_info.manuf_code = ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC;
  ESP_ERROR_CHECK(esp_zb_zcl_update_reporting_info(&humidity_reporting_info));
  log_i("UPDATE REPORTING INFO HUMIDITY");

  esp_zb_core_action_handler_register(zb_action_handler);

  esp_zb_set_primary_network_channel_set(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);

  ESP_ERROR_CHECK(esp_zb_start(false));

  esp_zb_main_loop_iteration();  // internal main loop!!!!!
}
/************************ Temp sensor *****************************/
static void esp_app_temp_sensor_handler(void *pvParameters) {
  while (1) {  // loop forever in task

    int16_t temp_measured_value = SCD30_temp * 100;        // value can't be float, *100 for 2 decimals
    float co2_measured_value = SCD30_CO2 / 1000000.0;      // value between 0...1
    uint16_t humidity_measured_value = SCD30_humid * 100;  // value can't be float, *100 for 2 decimals

    //int16_t measured_value = 500 * sin(0.005 * millis() / 1000) + 2000;

    /* Update temperature sensor measured value */
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_set_attribute_val(HA_ESP_TEMPERATURE_ENDPOINT, ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, &temp_measured_value, false);
    esp_zb_lock_release();

    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_set_attribute_val(HA_ESP_CO2_ENDPOINT, ESP_ZB_ZCL_CLUSTER_ID_CARBON_DIOXIDE_MEASUREMENT, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_CARBON_DIOXIDE_MEASUREMENT_MEASURED_VALUE_ID, &co2_measured_value, false);
    esp_zb_lock_release();

    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_set_attribute_val(HA_ESP_HUMIDITY_ENDPOINT, ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID, &humidity_measured_value, false);
    esp_zb_lock_release();

    log_i("Updating sensor value: %d C and %fPPM and %f", temp_measured_value, co2_measured_value, humidity_measured_value);
    vTaskDelay(SENSOR_UPDATE_INTERVAL * 1000 / portTICK_PERIOD_MS);  // delay alternative for Tasks. Dont use delay()!
  }
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);
  Serial.println("Starting");   // will be shown in the terminal
  Serial.setDebugOutput(true);  // show debug messages in Serial

  // Init RMT and leave light OFF
  neopixelWrite(LED_PIN, 0, 0, 0);

  // Start Temperature sensor reading task
  xTaskCreate(SCD30_sensor_handler, "SCD30_sensor_read", 4096, NULL, 10, NULL);

  // Wait for SCD30...
  vTaskDelay(100 / portTICK_PERIOD_MS);  // delay alternative for Tasks. Dont use delay()!

  // Start Zigbee task
  xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);

  // Start Temperature sensor writing task
  xTaskCreate(esp_app_temp_sensor_handler, "temp_sensor_write", 2048, NULL, 10, NULL);
}

void loop() {
  // nothing here, runs in task
  ;
}

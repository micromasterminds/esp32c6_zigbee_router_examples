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

// Based on Espressif Light Bulb Example
//
// Modified by MicroMasterMinds 🤯
//
// Hoffe es funzt auch 😂
//
// TESTING!!! FULLY CUSTOM ENDPOINT

#ifndef ZIGBEE_MODE_ZCZR  // Coordinator Device or Router Device
#error "Zigbee router mode is not selected in Tools->Zigbee mode"
#endif

#include "esp_zigbee_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ha/esp_zigbee_ha_standard.h"

#define LED_PIN RGB_BUILTIN

/* Zigbee configuration */
#define MAX_CHILDREN 10                 /* the max amount of connected devices */
#define INSTALLCODE_POLICY_ENABLE false /* enable the install code policy for security */
#define HA_ESP_LIGHT_ENDPOINT 10        /* esp light bulb device endpoint, used to process light controlling commands */

/* Attribute values in ZCL string format
 * The string should be started with the length of its own. */
#define MANUFACTURER_NAME "\x12" \
                          "MicroMasterMinds"
#define MODEL_IDENTIFIER "\x07" \
                         "Light"

/********************* Zigbee functions **************************/
static void createCustomZigbeeLightDevice() {
  uint8_t test_attr;
  test_attr = 0;

  /* basic cluster create with fully customized */
  esp_zb_attribute_list_t *esp_zb_basic_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BASIC);
  esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (void *)MANUFACTURER_NAME);
  esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (void *)MODEL_IDENTIFIER);
  esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, (void *)ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE);
  esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, (void *)ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE);

  /* identify cluster create with fully customized */
  esp_zb_attribute_list_t *esp_zb_identify_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);
  esp_zb_identify_cluster_add_attr(esp_zb_identify_cluster, ESP_ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID, (void *)ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE);

  /* create client role of the cluster */
  esp_zb_attribute_list_t *esp_zb_on_off_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ON_OFF);

  /* level cluster create with fully customized */
  esp_zb_attribute_list_t *esp_zb_level_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL);
  esp_zb_level_cluster_add_attr(esp_zb_level_cluster, ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_START_UP_CURRENT_LEVEL_ID, &test_attr);

  /* FULLY CUSTOMIZED CLUSTER AND ATTRIBUTE create with fully customized */
  //esp_zb_custom_cluster_add_custom_attr

  /* create cluster lists for this endpoint */
  esp_zb_cluster_list_t *esp_zb_cluster_list = esp_zb_zcl_cluster_list_create();
  esp_zb_cluster_list_add_basic_cluster(esp_zb_cluster_list, esp_zb_basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_identify_cluster(esp_zb_cluster_list, esp_zb_identify_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(esp_zb_cluster_list, esp_zb_on_off_client_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_identify_cluster(esp_zb_cluster_list, esp_zb_identify_client_cluster, ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_level_cluster(esp_zb_cluster_list, esp_zb_level_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  //esp_zb_cluster_list_add_custom_cluster
  
  esp_zb_ep_list_t *esp_zb_ep_list = esp_zb_ep_list_create();
  esp_zb_endpoint_config_t endpoint_config = {
    .endpoint = HA_ESP_LIGHT_ENDPOINT,
    .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
    .app_device_id = ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID,
    .app_device_version = 0
  };
  esp_zb_ep_list_add_ep(esp_zb_ep_list, esp_zb_cluster_list, endpoint_config);
  esp_zb_device_register(esp_zb_ep_list);
}

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

  createCustomZigbeeLightDevice();

  esp_zb_core_action_handler_register(zb_action_handler);

  esp_zb_set_primary_network_channel_set(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);

  ESP_ERROR_CHECK(esp_zb_start(false));

  esp_zb_main_loop_iteration();
}
/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);
  Serial.println("Starting");   // will be shown in the terminal
  Serial.setDebugOutput(true);  // show debug messages in Serial

  // Init RMT and leave light OFF
  neopixelWrite(LED_PIN, 0, 0, 0);

  // Start Zigbee task
  xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);
}

void loop() {
  // nothing here, runs in task
  ;
}

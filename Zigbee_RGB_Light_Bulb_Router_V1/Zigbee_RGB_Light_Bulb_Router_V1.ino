// Based on Espressif Light Bulb Example
// 
// Modified by MicroMasterMinds 🤯
//
// Hoffe es funzt auch 😂

#ifndef ZIGBEE_MODE_ZCZR  // Coordinator Device or Router Device
#error "Zigbee router mode is not selected in Tools->Zigbee mode"
#endif

#include "esp_zigbee_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ha/esp_zigbee_ha_standard.h"

#define LED_PIN RGB_BUILTIN

/* Zigbee configuration */
#define MAX_CHILDREN 10                                                  /* the max amount of connected devices */
#define INSTALLCODE_POLICY_ENABLE false                                  /* enable the install code policy for security */
#define HA_ESP_LIGHT_ENDPOINT 10                                         /* esp light bulb device endpoint, used to process light controlling commands */

/* Attribute values in ZCL string format
 * The string should be started with the length of its own. */
#define MANUFACTURER_NAME "\x12" \
                          "MicroMasterMinds"
#define MODEL_IDENTIFIER "\x07" \
                         "Light"

/********************* Zigbee functions **************************/
static void createZigbeeLightDevice() {
  /* Create customized Light endpoint */
  esp_zb_color_dimmable_light_cfg_t lightConfig = ESP_ZB_DEFAULT_COLOR_DIMMABLE_LIGHT_CONFIG();

  esp_zb_ep_list_t *endpointList = esp_zb_ep_list_create();
  esp_zb_endpoint_config_t endpointConfig = {};
  endpointConfig.endpoint = HA_ESP_LIGHT_ENDPOINT;
  endpointConfig.app_profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  endpointConfig.app_device_id = ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID;
  endpointConfig.app_device_version = 0;

  esp_zb_cluster_list_t *clusterList = esp_zb_zcl_cluster_list_create();
  esp_zb_attribute_list_t *basicCluster = esp_zb_basic_cluster_create(&lightConfig.basic_cfg);

  ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(basicCluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (void *)MANUFACTURER_NAME));
  ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(basicCluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (void *)MODEL_IDENTIFIER));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_basic_cluster(clusterList, basicCluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_identify_cluster_create(&(lightConfig.identify_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_on_off_cluster(clusterList, esp_zb_on_off_cluster_create(&lightConfig.on_off_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_level_cluster(clusterList, esp_zb_level_cluster_create(&lightConfig.level_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_color_control_cluster(clusterList, esp_zb_color_control_cluster_create(&lightConfig.color_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

  esp_zb_ep_list_add_ep(endpointList, clusterList, endpointConfig);

  /* Register the device */
  esp_zb_device_register(endpointList);
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

  createZigbeeLightDevice();

  esp_zb_core_action_handler_register(zb_action_handler);

  esp_zb_set_primary_network_channel_set(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);

  //esp_zb_zcl_reset_nvram_to_factory_default();

  //esp_zb_nvram_erase_at_start(true);

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

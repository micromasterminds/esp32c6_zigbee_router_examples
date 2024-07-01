//
// creating multiple endpoints
// 
// Endpoint 1:  createZigbeeTempSensorDevice
// Endpoint 2:  createZigbeeCo2SensorDevice
// Endpoint 3:  createZigbeeHumiditySensorDevice
// Endpoint 4:  createZigbeeLightDevice
//

static void createZigbeeTempSensorDevice(esp_zb_ep_list_t *endpointList) {
  /* Create customized temperature sensor endpoint */
  esp_zb_temperature_sensor_cfg_t sensorConfig = ESP_ZB_DEFAULT_TEMPERATURE_SENSOR_CONFIG();
  sensorConfig.temp_meas_cfg.min_value = ESP_TEMP_SENSOR_MIN_VALUE * 100;
  sensorConfig.temp_meas_cfg.max_value = ESP_TEMP_SENSOR_MAX_VALUE * 100;

  esp_zb_endpoint_config_t endpointConfig = {};
  endpointConfig.endpoint = HA_ESP_TEMPERATURE_ENDPOINT;  // Temperature sensor endpoint
  endpointConfig.app_profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  endpointConfig.app_device_id = ESP_ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID;
  endpointConfig.app_device_version = 0;

  esp_zb_cluster_list_t *clusterList = esp_zb_zcl_cluster_list_create();
  esp_zb_attribute_list_t *basicCluster = esp_zb_basic_cluster_create(&(sensorConfig.basic_cfg));

  ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(basicCluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (void *)MANUFACTURER_NAME));
  ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(basicCluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (void *)MODEL_IDENTIFIER));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_basic_cluster(clusterList, basicCluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_identify_cluster_create(&(sensorConfig.identify_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE));

  ESP_ERROR_CHECK(esp_zb_cluster_list_add_temperature_meas_cluster(clusterList, esp_zb_temperature_meas_cluster_create(&(sensorConfig.temp_meas_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

  esp_zb_ep_list_add_ep(endpointList, clusterList, endpointConfig);
}

static void createZigbeeCo2SensorDevice(esp_zb_ep_list_t *endpointList) {
  /* Create customized co2 sensor endpoint */
  esp_zb_carbon_dioxide_sensor_cfg_t sensorConfig = {};
  sensorConfig.basic_cfg.power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;
  sensorConfig.basic_cfg.zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
  sensorConfig.identify_cfg.identify_time = ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;
  sensorConfig.carbon_dioxide_meas_cfg.measured_value = ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MEASURED_VALUE_DEFAULT_VALUE;
  sensorConfig.carbon_dioxide_meas_cfg.min_measured_value = CARBON_DIOXIDE_MIN_VALUE / 1000000.0;  // PPM
  sensorConfig.carbon_dioxide_meas_cfg.max_measured_value = CARBON_DIOXIDE_MAX_VALUE / 1000000.0;  // PPM

  esp_zb_endpoint_config_t endpointConfig = {};
  endpointConfig.endpoint = HA_ESP_CO2_ENDPOINT;
  endpointConfig.app_profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  endpointConfig.app_device_id = ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID;
  endpointConfig.app_device_version = 0;

  esp_zb_cluster_list_t *clusterList = esp_zb_zcl_cluster_list_create();
  esp_zb_attribute_list_t *basicCluster = esp_zb_basic_cluster_create(&(sensorConfig.basic_cfg));

  ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(basicCluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (void *)MANUFACTURER_NAME));
  ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(basicCluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (void *)MODEL_IDENTIFIER));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_basic_cluster(clusterList, basicCluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_identify_cluster_create(&(sensorConfig.identify_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE));

  ESP_ERROR_CHECK(esp_zb_cluster_list_add_carbon_dioxide_measurement_cluster(clusterList, esp_zb_carbon_dioxide_measurement_cluster_create(&(sensorConfig.carbon_dioxide_meas_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

  esp_zb_ep_list_add_ep(endpointList, clusterList, endpointConfig);
}

static void createZigbeeHumiditySensorDevice(esp_zb_ep_list_t *endpointList) {
  esp_zb_humidity_sensor_cfg_t sensorConfig = {};
  sensorConfig.basic_cfg.power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;
  sensorConfig.basic_cfg.zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
  sensorConfig.identify_cfg.identify_time = ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;
  sensorConfig.humidity_meas_cfg.measured_value = ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_DEFAULT;
  sensorConfig.humidity_meas_cfg.min_value = HUMIDITY_MIN_VALUE;  // 0...95 %
  sensorConfig.humidity_meas_cfg.max_value = HUMIDITY_MAX_VALUE;  // 0...95 %

  esp_zb_endpoint_config_t endpointConfig = {};
  endpointConfig.endpoint = HA_ESP_HUMIDITY_ENDPOINT;
  endpointConfig.app_profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  endpointConfig.app_device_id = ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID;
  endpointConfig.app_device_version = 0;

  esp_zb_cluster_list_t *clusterList = esp_zb_zcl_cluster_list_create();
  esp_zb_attribute_list_t *basicCluster = esp_zb_basic_cluster_create(&(sensorConfig.basic_cfg));

  ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(basicCluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (void *)MANUFACTURER_NAME));
  ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(basicCluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (void *)MODEL_IDENTIFIER));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_basic_cluster(clusterList, basicCluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_identify_cluster_create(&(sensorConfig.identify_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
  ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE));

  ESP_ERROR_CHECK(esp_zb_cluster_list_add_humidity_meas_cluster(clusterList, esp_zb_humidity_meas_cluster_create(&(sensorConfig.humidity_meas_cfg)), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

  esp_zb_ep_list_add_ep(endpointList, clusterList, endpointConfig);
}

static void createZigbeeLightDevice(esp_zb_ep_list_t *endpointList) {
  /* Create customized Light endpoint */
  esp_zb_on_off_light_cfg_t lightConfig = ESP_ZB_DEFAULT_ON_OFF_LIGHT_CONFIG();

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

  esp_zb_ep_list_add_ep(endpointList, clusterList, endpointConfig);
}
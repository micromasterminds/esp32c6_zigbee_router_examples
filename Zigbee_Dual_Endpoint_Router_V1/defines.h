#define LED_PIN RGB_BUILTIN

/* Zigbee configuration */
#define MAX_CHILDREN 10                 /* the max amount of connected devices */
#define INSTALLCODE_POLICY_ENABLE false /* enable the install code policy for security */
#define HA_ESP_TEMPERATURE_ENDPOINT 15  /* esp temperature sensor device endpoint, used for temperature measurement */
#define HA_ESP_CO2_ENDPOINT 16          /* esp co2 sensor device endpoint, used for co2 measurement */
#define HA_ESP_LIGHT_ENDPOINT 10        /* esp light bulb device endpoint, used to process light controlling commands */

/* Sensor update interval */
#define SENSOR_UPDATE_INTERVAL (5) /* Seconds */

/* Temperature Sensor configuration */
#define ESP_TEMP_SENSOR_MIN_VALUE (-40) /* Temperature sensor min measured value (degree Celsius) */
#define ESP_TEMP_SENSOR_MAX_VALUE (70)  /* Temperature sensor max measured value (degree Celsius) */

/* Carbon Dioxide Sensor configuration */
#define ESP_CARBON_DIOXIDE_MIN_VALUE (400)
#define ESP_CARBON_DIOXIDE_MAX_VALUE (10000)

/* Attribute values in ZCL string format
 * The string should be started with the length of its own. */
#define MANUFACTURER_NAME "\x12" \
                          "MicroMasterMinds"
#define MODEL_IDENTIFIER "\x0A" \
                         "Gas-Sensor"

/**
 * @brief Zigbee carbon dioxide measurement cluster.
 *
 */
typedef struct esp_zb_carbon_dioxide_measurement_sensor_cfg_s {
    esp_zb_basic_cluster_cfg_t basic_cfg;                /*!<  Basic cluster configuration, @ref esp_zb_basic_cluster_cfg_s */
    esp_zb_identify_cluster_cfg_t identify_cfg;          /*!<  Identify cluster configuration, @ref esp_zb_identify_cluster_cfg_s */
    esp_zb_carbon_dioxide_measurement_cluster_cfg_t carbon_dioxide_meas_cfg; /*!<  Carbon Dioxide measurement cluster configuration, @ref esp_zb_carbon_dioxide_meas_cluster_cfg_s */
} esp_zb_carbon_dioxide_sensor_cfg_t;


// /**
//  * @brief Zigbee standard mandatory attribute for humidity measurement cluster.
//  *
//  */
// typedef struct esp_zb_humidity_measurement_sensor_cfg_s {
//     esp_zb_basic_cluster_cfg_t basic_cfg;                /*!<  Basic cluster configuration, @ref esp_zb_basic_cluster_cfg_s */
//     esp_zb_identify_cluster_cfg_t identify_cfg;          /*!<  Identify cluster configuration, @ref esp_zb_identify_cluster_cfg_s */
//     esp_zb_humidity_meas_cluster_cfg_t humidity_meas_cfg; /*!<  Carbon Dioxide measurement cluster configuration, @ref esp_zb_carbon_dioxide_meas_cluster_cfg_s */
// } esp_zb_humidity_sensor_cfg_t;
//
//  Action Handler for Light Bulb 
//

static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
  esp_err_t ret = ESP_OK;
  switch (callback_id) {
    case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID: ret = zb_attribute_handler((esp_zb_zcl_set_attr_value_message_t *)message); break;
    default: log_w("Receive Zigbee action(0x%x) callback", callback_id); break;
  }
  return ret;
}

/* Handle the light attribute */
static esp_err_t zb_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
  esp_err_t ret = ESP_OK;
  bool light_state = 0;

  if (!message) {
    log_e("Empty message");
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
  }

  log_i(
    "Received message: endpoint(%d), cluster(0x%x), attribute(0x%x), data size(%d)", message->info.dst_endpoint, message->info.cluster, message->attribute.id,
    message->attribute.data.size);
  if (message->info.dst_endpoint == HA_ESP_LIGHT_ENDPOINT) {
    if (message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) {
      if (message->attribute.id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID && message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL) {
        light_state = message->attribute.data.value ? *(bool *)message->attribute.data.value : light_state;
        log_i("Light sets to %s", light_state ? "On" : "Off");
        neopixelWrite(LED_PIN, 255 * light_state, 255 * light_state, 255 * light_state);  // Toggle light
      }
    }
  }
  return ret;
}

//
// action handler
//

esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
  esp_err_t ret = ESP_OK;
  switch (callback_id) {
    case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:
      ret = zb_attribute_handler((esp_zb_zcl_set_attr_value_message_t *)message);
      break;
    default:
      log_w("Receive Zigbee action(0x%x) callback", callback_id);
      break;
  }
  return ret;
}

esp_err_t zb_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
  esp_err_t ret = ESP_OK;
  bool light_state = 0;
  uint8_t light_level = 0;
  uint16_t light_color_x = 0;
  uint16_t light_color_y = 0;
  if (message == NULL)
    log_e("Empty message!");
  if (message->info.status == ESP_ZB_ZCL_STATUS_SUCCESS)
    log_e("Received message: error status(%d)", message->info.status);

  log_i("Received message: endpoint(%d), cluster(0x%x), attribute(0x%x), data size(%d)", message->info.dst_endpoint, message->info.cluster,
        message->attribute.id, message->attribute.data.size);


  if (message->info.dst_endpoint == HA_ESP_LIGHT_ENDPOINT) {
    switch (message->info.cluster) {
      
      case ESP_ZB_ZCL_CLUSTER_ID_ON_OFF:
        if (message->attribute.id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID && message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL) {
          light_state = message->attribute.data.value ? *(bool *)message->attribute.data.value : light_state;
          log_i("Light sets to %s", light_state ? "On" : "Off");
          light_driver_set_power(light_state);
        } else {
          log_w("On/Off cluster data: attribute(0x%x), type(0x%x)", message->attribute.id, message->attribute.data.type);
        }
        break;

      case ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL:
        if (message->attribute.id == ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID && message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
          light_color_x = message->attribute.data.value ? *(uint16_t *)message->attribute.data.value : light_color_x;
          light_color_y = *(uint16_t *)esp_zb_zcl_get_attribute(message->info.dst_endpoint, message->info.cluster,
                                                                ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID)
                             ->data_p;
          log_i("Light color x changes to 0x%x", light_color_x);
        } else if (message->attribute.id == ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID && message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
          light_color_y = message->attribute.data.value ? *(uint16_t *)message->attribute.data.value : light_color_y;
          light_color_x = *(uint16_t *)esp_zb_zcl_get_attribute(message->info.dst_endpoint, message->info.cluster,
                                                                ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID)
                             ->data_p;
          log_i("Light color y changes to 0x%x", light_color_y);
        } else {
          log_w("Color control cluster data: attribute(0x%x), type(0x%x)", message->attribute.id, message->attribute.data.type);
        }
        light_driver_set_color_xy(light_color_x, light_color_y);
        break;

      case ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL:
        if (message->attribute.id == ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID && message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_U8) {
          light_level = message->attribute.data.value ? *(uint8_t *)message->attribute.data.value : light_level;
          light_driver_set_level((uint8_t)light_level);
          log_i("Light level changes to %d", light_level);
        } else {
          log_w("Level Control cluster data: attribute(0x%x), type(0x%x)", message->attribute.id, message->attribute.data.type);
        }
        break;
        
      default:
        log_i("Message data: cluster(0x%x), attribute(0x%x)  ", message->info.cluster, message->attribute.id);
    }
  }
  return ret;
}
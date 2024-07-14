/** Convert Hue,Saturation,V to RGB
 * RGB - [0..0xffff]
 * hue - [0..0xff]
 * Sat - [0..0xff]
 * V always = (ZB_UINT16_MAX-1)
*/
#define HSV_to_RGB(h, s, v, r, g, b) \
  { \
    uint8_t i; \
    uint8_t sector = UINT8_MAX / 6; \
    float f, p, q, t; \
    if (s == 0) { /* achromatic (grey)*/ \
      r = g = b = (v); \
    } else { \
      i = h / sector; /* sector 0 to 5 */ \
      f = h % sector; /* factorial part of h*/ \
      p = (float)(v * (1.0 - (float)s / UINT8_MAX)); \
      q = (float)(v * (1.0 - (float)s / UINT8_MAX * f / (float)sector)); \
      t = (float)(v * (1.0 - (float)s / UINT8_MAX * (1 - f / (float)sector))); \
      switch (i) { \
        case 0: \
          r = (v); \
          g = t; \
          b = p; \
          break; \
        case 1: \
          r = q; \
          g = (v); \
          b = p; \
          break; \
        case 2: \
          r = p; \
          g = (v); \
          b = t; \
          break; \
        case 3: \
          r = p; \
          g = q; \
          b = (v); \
          break; \
        case 4: \
          r = t; \
          g = p; \
          b = (v); \
          break; \
        case 5: \
        default: \
          r = (v); \
          g = p; \
          b = q; \
          break; \
      } \
    } \
  }

#define XYZ_to_RGB(X, Y, Z, r, g, b) \
  { \
    r = (float)(3.240479 * (X)-1.537150 * (Y)-0.498535 * (Z)); \
    g = (float)(-0.969256 * (X) + 1.875992 * (Y) + 0.041556 * (Z)); \
    b = (float)(0.055648 * (X)-0.204043 * (Y) + 1.057311 * (Z)); \
    if (r > 1) { r = 1; } \
    if (g > 1) { g = 1; } \
    if (b > 1) { b = 1; } \
  }

// static led_strip_handle_t s_led_strip;
static uint8_t s_red = 255, s_green = 255, s_blue = 255, s_level = 255;

void light_driver_set_color_xy(uint16_t color_current_x, uint16_t color_current_y) {
  float red_f = 0, green_f = 0, blue_f = 0, color_x, color_y;
  color_x = (float)color_current_x / 65535;
  color_y = (float)color_current_y / 65535;
  /* assume color_Y is full light level value 1  (0-1.0) */
  float color_X = color_x / color_y;
  float color_Z = (1 - color_x - color_y) / color_y;
  /* change from xy to linear RGB NOT sRGB */
  XYZ_to_RGB(color_X, 1, color_Z, red_f, green_f, blue_f);
  float ratio = (float)s_level / 255;
  s_red = (uint8_t)(red_f * (float)255);
  s_green = (uint8_t)(green_f * (float)255);
  s_blue = (uint8_t)(blue_f * (float)255);
  neopixelWrite(RGB_BUILTIN, s_red * ratio, s_green * ratio, s_blue * ratio);
  // ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, s_red * ratio, s_green * ratio, s_blue * ratio));
  // ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_set_color_hue_sat(uint8_t hue, uint8_t sat) {
  float red_f, green_f, blue_f;
  HSV_to_RGB(hue, sat, UINT8_MAX, red_f, green_f, blue_f);
  float ratio = (float)s_level / 255;
  s_red = (uint8_t)red_f;
  s_green = (uint8_t)green_f;
  s_blue = (uint8_t)blue_f;
  neopixelWrite(RGB_BUILTIN, s_red * ratio, s_green * ratio, s_blue * ratio);
  // ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, s_red * ratio, s_green * ratio, s_blue * ratio));
  // ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_set_color_RGB(uint8_t red, uint8_t green, uint8_t blue) {
  float ratio = (float)s_level / 255;
  s_red = red;
  s_green = green;
  s_blue = blue;
  neopixelWrite(RGB_BUILTIN, s_red * ratio, s_green * ratio, s_blue * ratio);
  // ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, red * ratio, green * ratio, blue * ratio));
  // ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_set_power(bool power) {
  neopixelWrite(RGB_BUILTIN, s_red * power, s_green * power, s_blue * power);
  // ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, s_red * power, s_green * power, s_blue * power));
  // ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_set_level(uint8_t level) {
  s_level = level;
  float ratio = (float)s_level / 255;
  neopixelWrite(RGB_BUILTIN, s_red * ratio, s_green * ratio, s_blue * ratio);
  // ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, s_red * ratio, s_green * ratio, s_blue * ratio));
  // ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

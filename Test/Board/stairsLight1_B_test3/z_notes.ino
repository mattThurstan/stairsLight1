/*----------------------------notes----------------------------*/

/* 
 * WeMos D1 (R2 &) mini, 80 MHz, 115200 baud, 4M, (3M SPIFFS)
 * 
 * http://wiki.keyestudio.com/index.php/Ks0052_keyestudio_PIR_Motion_Sensor
 * 
 * LED strips
 * 
 */

/*
 * Home Assistant config 
 * 
 *   light:
 *     - platform: mqtt
 *       name: 'Stairs lights'
 *       state_topic: 'house/stairs/lights/light/status'
 *       command_topic: 'house/stairs/lights/light/switch'
 *       state_value_template: "{{ value_json.state }}"
 *       brightness_state_topic: 'house/stairs/lights/brightness/status'
 *       brightness_command_topic: 'house/stairs/lights/brightness/set'
 *       brightness_value_template: "{{ value_json.brightness }}"
 *       hue_state_topic: 'house/stairs/lights/hue/status'
 *       hue_command_topic: 'house/stairs/lights/hue/set'
 *       rgb_state_topic: 'house/stairs/lights/rgb/status'
 *       rgb_command_topic: 'house/stairs/lights/rgb/set'
 *       rgb_value_template: "{{ value_json.rgb | join(,) }}"
 *       qos: 0
 *       payload_on: "ON"
 *       payload_off: "OFF"
 *       optimistic: false
 */
 

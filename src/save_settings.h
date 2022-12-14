#include "Arduino.h"
#include "EEPROM.h"


#define bytes_used_eeprom           9

#define check_eeprom_addr           0

#define temp_set_addr               1
#define humidity_set_addr           2

#define timer_hours_set_addr        3
#define timer_minutes_set_addr      4

#define heater_state_addr           5
#define light_state_addr            6
#define fan_state_addr              7
#define vapor_state_addr            8

#define check_eeprom_value          85


void eeprom_settings_init();
bool eeprom_check();
bool save(uint8_t addr, uint8_t value);

bool save_temp_set(uint8_t temp);
bool save_humidity_set(uint8_t humidity);
bool save_timer_set(uint8_t hours, uint8_t minutes);
bool save_heater_state(bool state);
bool save_light_state(bool state);
bool save_button_cooler_state(bool state);
bool save_vapor_state(bool state);

uint8_t read_eeprom_temp_set();
uint8_t read_eeprom_humidity_set();
uint8_t read_eeprom_timer_hours_set();
uint8_t read_eeprom_timer_minutes_set();
bool read_eeprom_heater_state();
bool read_eeprom_light_state();
bool read_eeprom_fan_state();
bool read_eeprom_vapor_state();

void read_and_print_eeprom_settings();

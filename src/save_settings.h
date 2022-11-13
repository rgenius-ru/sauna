#include "Arduino.h"
#include "EEPROM.h"


#define bytes_used_eeprom 3

#define check_eeprom_addr 0
#define temp_set_addr 1
#define humidity_set_addr 2

#define check_eeprom_value 85


void eeprom_settings_init();
bool eeprom_check();
bool save(uint8_t addr, uint8_t value);

bool save_temp_set(uint8_t temp);
bool save_humidity_set(uint8_t humidity);

uint8_t read_eeprom_temp_set();
uint8_t read_eeprom_humidity_set();

void read_and_print_eeprom_settings();

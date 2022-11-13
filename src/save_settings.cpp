#include "save_settings.h"


/* Data to saving:
    - check_eeprom  1 byte  uint8_t  
    - temp_set      1 byte  uint8_t
    - humidity_set  1 byte  uint8_t
*/ 


void eeprom_settings_init(){
    EEPROM.begin(bytes_used_eeprom);
}

bool save(uint8_t addr, uint8_t value){
    EEPROM.put(addr, value);

    if (EEPROM.commit()) {
        return 0; // no errors
    }
    
    Serial.println("ERROR! EEPROM commit failed");
    return 1; // ERROR. EEPROM commit failed
}

bool eeprom_check(){
    uint8_t data_from_eeprom = 0xff;
    EEPROM.get(check_eeprom_addr, data_from_eeprom); 

    return data_from_eeprom == check_eeprom_value;
}

bool save_temp_set(uint8_t temp){
    return save(temp_set_addr, temp);
}


bool save_humidity_set(uint8_t humidity){
    return save(humidity_set_addr, humidity);
}

uint8_t read_eeprom_temp_set(){
    uint8_t temp = 0xff;
    EEPROM.get(temp_set_addr, temp); 

    return temp;
}

uint8_t read_eeprom_humidity_set(){
    uint8_t humidity = 0xff;
    EEPROM.get(humidity_set_addr, humidity); 

    return humidity;
}

void read_and_print_eeprom_settings(){
    String s = "";

    s += "temp_set C: " + String(read_eeprom_temp_set());
    s += "\thumidity_set %: " + String(read_eeprom_humidity_set());

    Serial.println(s);
}
#include "physical_quantity.h"
#include "nextion.h"


PhysicalQuantity::PhysicalQuantity(String name, uint8_t min, uint8_t max){
    _name = name;
    max_value = max;
    min_value = min;
}

void PhysicalQuantity::set(uint8_t value, bool display_update){
        // Serial.println(value);
        // Serial.println(min_value);
        // Serial.println(max_value);
    if (value >= min_value and value <= max_value){
        _value = value;
        _value_str = String(_value);

        if (display_update)
        {
            change_text(_name, _value_str);
        }
    }
}

uint8_t PhysicalQuantity::get(){
    return _value;
}


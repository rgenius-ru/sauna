#if !defined(PHYSICAL_QUANTITY_H)
#define PHYSICAL_QUANTITY_H


#include "Arduino.h"


class PhysicalQuantity{
    public:
        PhysicalQuantity(String name, uint8_t min, uint8_t max);

        uint8_t max_value;
        uint8_t min_value;

        void set(uint8_t value);
        uint8_t get();

    private:
        uint8_t _value;
        String _value_str;
        String _name;
};


#endif // PHYSICAL_QUANTITY_H

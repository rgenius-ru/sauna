#if !defined(ERRORS_H)
#define ERRORS_H


#include "Arduino.h"


#define ERROR_NO_ERROR 0
#define ERROR_TEMP_SENSOR_DISCONNECTED 1
#define ERROR_TEMP_SENSOR_OVERHEAT 2

#define ERROR_TEMP_SENSOR_DISCONNECTED_TEXT "Error: Could not read temperature data"
#define ERROR_TEMP_SENSOR_OVERHEAT_TEXT "Error: Overheat temperature" 


String get_error_text(uint8_t error_id);


#endif // ERRORS_H

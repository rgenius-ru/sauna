#include "errors.h"


String get_error_text(uint8_t error_id)
{
    String error_text = "";

    switch (error_id)
    {
    case ERROR_NO_ERROR:
        error_text = "";
        break;
    
    case ERROR_TEMP_SENSOR_DISCONNECTED:
        error_text = ERROR_TEMP_SENSOR_DISCONNECTED_TEXT;
        break;

    case ERROR_TEMP_SENSOR_OVERHEAT:
        error_text = ERROR_TEMP_SENSOR_OVERHEAT_TEXT;
        break;

    default:
        break;
    }

    return error_text;
}
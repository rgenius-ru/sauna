#if !defined(ERRORS_H)
#define ERRORS_H


#include "Arduino.h"


#define ERROR_TEMP_SENSOR_DISCONNECTED "Датчик температуры отключен" //"Error: Could not read temperature data"
#define ERROR_TEMP_SENSOR_OVERHEAT "Перегрев печи (t > 100 С)" // "Error: Overheat temperature" 


struct Error {
  bool has_error;
  String text;
};


#endif // ERRORS_H

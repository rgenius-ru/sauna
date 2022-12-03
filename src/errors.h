#if !defined(ERRORS_H)
#define ERRORS_H


#include "Arduino.h"


#define ERROR_TEMP_SENSOR_DISCONNECTED "Датчик температуры отключен" // "Error: Could not read temperature data"
#define ERROR_TEMP_SENSOR_OVERHEAT "Перегрев воздуха" // "Error: Overheat temperature" 

#define ERROR_HUMIDITY_SENSOR_DISCONNECTED "Датчик влажности отключен" // "Failed to read from DHT sensor!"
#define ERROR_HUMIDITY_SENSOR_OVERVAPOR "Переувлажнение воздуха"

struct Error {
  bool has_error;
  String text;
};


#endif // ERRORS_H

#if !defined(ERRORS_H)
#define ERRORS_H


#include "Arduino.h"


#define ERROR_TEMP_SENSOR_DISCONNECTED "Датчик температуры отключен" // "Error: Could not read temperature data"
#define ERROR_TEMP_SENSOR_OVERHEAT "Перегрев воздуха" // "Error: Overheat temperature" 
#define ERROR_HEATER "Отсутствует нагрев воздуха"

#define ERROR_HUMIDITY_SENSOR_DISCONNECTED "Датчик влажности отключен" // "Failed to read from DHT sensor!"
#define ERROR_HUMIDITY_SENSOR_OVERVAPOR "Переувлажнение воздуха"
#define ERROR_VAPOR "Отсутствует увлажнение воздуха"
#define ERROR_MOTOR "Неисправен выдвижной механизм"

struct Error {
  bool has_error;
  String text;
};


#endif // ERRORS_H

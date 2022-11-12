#if !defined(NEXTION_H)
#define NEXTION_H


#include "Arduino.h"


bool read_nextion(uint8_t &obj, bool &state);
void write_nextion(String command);
void change_pic(String obj_name, uint8_t id_pic);
void change_text(String obj_name, String text);


#endif // NEXTION_H

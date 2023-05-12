#if !defined(NEXTION_H)
#define NEXTION_H


#include "Arduino.h"


#define LOGO_PAGE_ID 0
#define MAIN_PAGE_ID 1
#define ERROR_PAGE_ID 2


bool read_nextion(uint8_t &obj, bool &state);
void write_nextion(String command);
void change_page_to(uint8_t page_id);
void change_pic(String obj_name, uint8_t id_pic);
void change_pic(String obj_name, uint8_t id_pic, bool is_need_print);
void change_text(String obj_name, String text);
void change_color(String obj_name, uint32_t color);
void error_show(String error_text);


#endif // NEXTION_H

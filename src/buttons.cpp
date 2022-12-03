#include "buttons.h"
#include "nextion.h"
#include "common.h"

// ************************************ Button ************************************************
Button::Button(String obj_name) {
    _obj_name = obj_name;
}

void Button::_change_pics(uint8_t pic, uint8_t pic2) {
    change_pic(_obj_name + ".pic", pic);
    change_pic(_obj_name + ".pic2", pic2);
}

void Button::set_pics(uint8_t pic, uint8_t pic2, uint8_t disabled_pic) {
    _pic = pic;
    _pic2 = pic2;

    _disabled_pic = disabled_pic;
}

void Button::init(bool enabled, bool need_change_pics) {
    _enabled = enabled;
    _press_state = RELEASED;

    if (need_change_pics)
    {
        if (_enabled){
            _change_pics(_pic, _pic2);
        }
        else{
            _change_pics(_disabled_pic, _disabled_pic);
        }
    }
}

void Button::set_press_state(bool state){
    _press_state = state;
}

void Button::enable() {
    _enabled = true;  
    _change_pics(_pic, _pic2);
}

void Button::disable() {
    _enabled = false;
    _change_pics(_disabled_pic, _disabled_pic);
}

bool Button::is_enable(){
    return _enabled;
}

bool Button::is_pressed(){
    return _press_state;
}


// ********************************** LatchingButton ******************************************
void LatchingButton::set_pics(uint8_t on_pic, uint8_t on_pic2, uint8_t off_pic, uint8_t off_pic2, uint8_t disabled_pic) {
    _on_pic = on_pic;
    _on_pic2 = on_pic2;

    _off_pic = off_pic;
    _off_pic2 = off_pic2;

    _disabled_pic = disabled_pic;
}

void LatchingButton::init(bool enabled, bool need_change_pics) {
    _enabled = enabled;
    _press_state = RELEASED;
    _on = OFF;
    
    if (_enabled){
        _now_pic = _off_pic;
        _now_pic2 = _off_pic2;
    }
    else{
        _now_pic = _disabled_pic;
        _now_pic2 = _disabled_pic;
    }
    
    _change_pics(_now_pic, _now_pic2);
}

void LatchingButton::on() {
    _on = ON;

    _now_pic = _on_pic;
    _now_pic2 = _on_pic2;

    _change_pics(_now_pic, _now_pic2);
}

void LatchingButton::off() {
    _on = OFF;

    _now_pic = _off_pic;
    _now_pic2 = _off_pic2;

    _change_pics(_now_pic, _now_pic2);
}

void LatchingButton::inverse_on_off_state(){
    if (_on){
        off();
    }
    else{
        on();
    }
}

void LatchingButton::set_state(bool state){
    if (state){
        on();
    }
    else{
        off();
    }
}

void LatchingButton::enable() {
    _enabled = true;

    if (_on){
        _now_pic = _on_pic;
        _now_pic2 = _on_pic2;
    }
    else{
        _now_pic = _off_pic;
        _now_pic2 = _off_pic2;
    }
    
    _change_pics(_now_pic, _now_pic2);
}

void LatchingButton::disable() {
    _enabled = false;

    _now_pic = _disabled_pic;
    _now_pic2 = _disabled_pic;

    _change_pics(_now_pic, _now_pic2);
}

bool LatchingButton::is_on(){
    return _on;
}

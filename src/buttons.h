#if !defined(BUTTONS_H)
#define BUTTONS_H


#include "Arduino.h"


class Button {
    public:
        Button(String obj_name);
        void set_pics(uint8_t pic, uint8_t pic2, uint8_t disabled_pic);
        void init(bool enabled, bool need_change_pics = false);
        
        void enable();
        void disable();
        bool is_enable();
        
        void set_press_state(bool state);
        bool is_pressed();

    protected:
        String _obj_name;

        uint8_t _disabled_pic;

        bool _enabled;
        bool _press_state;

        void _change_pics(uint8_t pic, uint8_t pic2);

    private:
        uint8_t _pic;
        uint8_t _pic2;
};

class LatchingButton : public Button {
    public:
        using Button::Button;
        void set_pics(uint8_t on_pic, uint8_t on_pic2, uint8_t off_pic, uint8_t off_pic2, uint8_t disabled_pic);
        void init(bool enabled, bool need_change_pics = false);
        
        void enable();
        void disable();
        
        void on();
        void off();
        void inverse_on_off_state();
        bool is_on();

    private:
        uint8_t _now_pic;
        uint8_t _now_pic2;

        uint8_t _on_pic;
        uint8_t _on_pic2;

        uint8_t _off_pic;
        uint8_t _off_pic2;

        bool _on;
};


#endif // BUTTONS_H

#include "Arduino.h"
#include "images.h"
#include "nextion.h"
#include "buttons.h"
#include "common.h"
#include "physical_quantity.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "DHT.h"
#include "save_settings.h"


#define RESERVED_RELAY_PIN 14

#define LED_PIN 2
#define LIGHT_PIN 19
#define FAN_PIN 27
#define MOTOR_ENABLE_PIN 18
#define MOTOR_A_PIN 21
#define MOTOR_B_PIN 5
#define VAPOR_PIN 25
#define HEATER_PIN 26
#define MAX_ENDSTOP_PIN 34
#define MIN_ENDSTOP_PIN 35
#define TEMP_SENSOR_PIN 32
#define HUMIDITY_SENSOR_PIN 33
#define BUZZER_PIN 4

#define TEMP_MIN 50
#define TEMP_MAX 80
#define HUMIDITY_MIN 40
#define HUMIDITY_MAX 80

#define HH_MAX 23   // hours
#define HH_MIN 0    // hours
#define MM_MAX 59   // minutes
#define MM_MIN 0    // minutes

#define LIGHT_FLASH_TIMES 3
#define BUZZER_FLASH_TIMES 3

#define DHTTYPE DHT21 // DHT 21 (AM2301)

#define TEXT_COLOR_DEFAULT 63320 // #F0E8C0 rgb(240,232,192)
#define TEXT_COLOR_SELECTED 6371 // 1791 // #00DEFF rgb(0,222,255) 1631 // #00C8F8 rgb(0,200,248)
#define TEXT_COLOR_DISABLED 6371 // #1C1D1F rgb(28,29,31)

#define COLON ":"

OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature temp_sensor(&oneWire);

DHT humidity_sensor(HUMIDITY_SENSOR_PIN, DHTTYPE);

// bool button1_state = OFF;
bool max_endstop_state = OPEN;
bool min_endstop_state = OPEN;
bool motor_enable = OFF;
bool motor_direction = DIRECTION_TO_MAX;
bool vapor_state = OFF;
bool heater_state = OFF;
bool fan_state = OFF;
bool light_state = OFF;
bool temp_selected_state = false;
bool humidity_selected_state = false;
bool timer_selected_state = false;
bool timer_state = false;
bool timer_need_save = false;

uint32_t endstop_previous_time = 0;
uint16_t endstop_interval_update = 50; // ms

uint32_t temp_previous_time = 0;
uint16_t temp_interval_update = 2000; // ms

uint32_t humidity_previous_time = 0;
uint16_t humidity_interval_update = 2000; // ms

uint32_t heater_previous_time = 0;
uint16_t heater_interval_update = 1000; // ms

uint32_t vapor_previous_time = 0;
uint16_t vapor_interval_update = 1000; // ms

uint32_t fan_previous_time = 0;
uint16_t fan_interval_update = 1000; // ms

uint32_t temp_selected_previous_time = 0;
uint16_t temp_selected_timer = 3000; // ms

uint32_t humidity_selected_previous_time = 0;
uint16_t humidity_selected_timer = 3000; // ms

uint32_t timer_selected_previous_time = 0;
uint16_t timer_selected_timer = 3000; // ms

uint32_t timer_previous_time = 0;
uint16_t timer_interval_update = 60000; // ms

uint32_t colon_previous_time = 0;
uint16_t colon_interval_update = 500; // ms

uint32_t light_previous_time = 0;
uint16_t light_flash_interval = 1000; // ms

uint32_t buzzer_previous_time = 0;
uint16_t buzzer_flash_interval = 1000; // ms

uint16_t light_flash_count = 0;
uint16_t buzzer_flash_count = 0;

String colon = COLON;

LatchingButton power_button = LatchingButton("power");
LatchingButton move_open_button = LatchingButton("move_open");
LatchingButton move_close_button = LatchingButton("move_close");

LatchingButton vapor_button = LatchingButton("vap");
LatchingButton fan_button = LatchingButton("fan");
LatchingButton heat_button = LatchingButton("heat");
LatchingButton light_button = LatchingButton("light");

Button t_up_button = Button("t_up");
Button t_down_button = Button("t_down");
Button h_up_button = Button("h_up");
Button h_down_button = Button("h_down");

Button hh_up_button = Button("hh_up");
Button hh_down_button = Button("hh_down");
Button mm_up_button = Button("mm_up");
Button mm_down_button = Button("mm_down");

PhysicalQuantity temp = PhysicalQuantity("t", 0, 99);
PhysicalQuantity humidity = PhysicalQuantity("h", 0, 99);

uint8_t temp_set = 0;
uint8_t humidity_set = 0;

uint8_t timer_time = 0;
uint8_t timer_hours = 0;
uint8_t timer_minutes = 0;


void change_to_page1()
{
  Serial.println();
  Serial.println();
  Serial.println();

  // read nextion
  while (Serial2.available() > 0) {
    // uint8_t byte_read = Serial2.read();
    // Serial.print(byte_read, HEX);
    Serial.println(Serial2.readString());
  }
  Serial.println();

  for (uint8_t i=0; i<10; i++)
  {
    Serial2.print("page 1");
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    Serial2.write(0xFF);
    delay(500);

    String str = "";
    while (Serial2.available() > 0) {
      str += char(Serial2.read());
      // Serial.println(Serial2.readString());
    }
    Serial.println(str);

    if (str == "page1 initialized")
    {
      break;
    }

    Serial.println();
  }
}

String format_two_digits(uint8_t number)
{
  String str = "";

  if (number < 10)
  {
    str += "0";
  }

  str += String(number);

  return str;
}

void disable_all_objects()
{
  // Off objects
  digitalWrite(LIGHT_PIN, HIGH);
  digitalWrite(FAN_PIN, HIGH);
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
  digitalWrite(MOTOR_A_PIN, HIGH);
  digitalWrite(MOTOR_B_PIN, LOW);
  digitalWrite(VAPOR_PIN, LOW);
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  vapor_state = OFF;
  heater_state = OFF;
  fan_state = OFF;
}

void init_all_pins()
{
  // Init pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_A_PIN, OUTPUT);
  pinMode(MOTOR_B_PIN, OUTPUT);
  pinMode(VAPOR_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TEMP_SENSOR_PIN, INPUT_PULLUP);
  pinMode(HUMIDITY_SENSOR_PIN, INPUT_PULLUP);

  pinMode(MAX_ENDSTOP_PIN, INPUT_PULLUP);
  pinMode(MIN_ENDSTOP_PIN, INPUT_PULLUP);
}

void buttons_init()
{
  power_button.set_pics(big_on_normal, big_on_pressed, big_off_normal, big_off_pressed, big_disable_normal);
  move_open_button.set_pics(open_on_normal, open_on_pressed, open_off_normal, open_off_pressed, open_disable_normal);
  move_close_button.set_pics(close_on_normal, close_on_pressed, close_off_normal, close_off_pressed, close_disable_normal);

  vapor_button.set_pics(vapor_on_normal, vapor_on_pressed, vapor_off_normal, vapor_off_pressed, vapor_disable_normal);
  fan_button.set_pics(cooler_on_normal, cooler_on_pressed, cooler_off_normal, cooler_off_pressed, cooler_disable_normal);
  heat_button.set_pics(heater_on_normal, heater_on_pressed, heater_off_normal, heater_off_pressed, heater_disable_normal);
  light_button.set_pics(light_on_normal, light_on_pressed, light_off_normal, light_off_pressed, light_disable_normal);

  t_up_button.set_pics(up_normal, up_pressed, up_disable);
  t_down_button.set_pics(down_normal, down_pressed, down_disable);
  h_up_button.set_pics(up_normal, up_pressed, up_disable);
  h_down_button.set_pics(down_normal, down_pressed, down_disable);

  hh_up_button.set_pics(up_normal, up_pressed, up_disable);
  hh_down_button.set_pics(down_normal, down_pressed, down_disable);
  mm_up_button.set_pics(up_normal, up_pressed, up_disable);
  mm_down_button.set_pics(down_normal, down_pressed, down_disable);

  power_button.init(ENABLE, true);
  move_open_button.init(ENABLE, true);
  move_close_button.init(DISABLE, true);

  vapor_button.init(DISABLE, true);
  fan_button.init(DISABLE, true);
  heat_button.init(DISABLE, true);
  light_button.init(DISABLE, true);

  t_up_button.init(DISABLE);
  t_down_button.init(DISABLE);
  h_up_button.init(DISABLE);
  h_down_button.init(DISABLE);

  hh_up_button.init(DISABLE);
  hh_down_button.init(DISABLE);
  mm_up_button.init(DISABLE);
  mm_down_button.init(DISABLE);
}

void button_update(LatchingButton &button, bool state)
{
  if (state == PRESSED)
  {
    if (button.is_enable())
    {
      button.inverse_on_off_state();
    }
  }

  button.set_press_state(state);
}

void max_endstop_update()
{
  bool state = digitalRead(MAX_ENDSTOP_PIN);

  if (state != max_endstop_state)
  {
    max_endstop_state = state;
    Serial.println(max_endstop_state);
  }
}

void min_endstop_update()
{
  bool state = digitalRead(MIN_ENDSTOP_PIN);

  if (state != min_endstop_state)
  {
    min_endstop_state = state;
    Serial.println(min_endstop_state);
  }
}

void endstops_update()
{
  if (millis() > endstop_previous_time + endstop_interval_update)
  {
    endstop_previous_time = millis();

    max_endstop_update();
    min_endstop_update();
  }
}

void motor_on()
{
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
  motor_enable = ON;
  Serial.println("motor_on");
}

void motor_off()
{
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
  motor_enable = OFF;
  Serial.println("motor_off");
}

void motor_set_direction(bool direction)
{
  if (direction == DIRECTION_TO_MAX)
  {
    digitalWrite(MOTOR_A_PIN, HIGH);
    digitalWrite(MOTOR_B_PIN, LOW);
  }
  else
  {
    digitalWrite(MOTOR_A_PIN, LOW);
    digitalWrite(MOTOR_B_PIN, HIGH);
  }
}

void motor_update()
{
  if (motor_enable){
    if (motor_direction == DIRECTION_TO_MAX and max_endstop_state == CLOSE)
    {
      motor_off();
      move_open_button.off();
      motor_direction = DIRECTION_TO_MIN;
      motor_set_direction(motor_direction);
    }else if(motor_direction == DIRECTION_TO_MIN and min_endstop_state == CLOSE)
    {
      motor_off();
      move_open_button.off();
      motor_direction = DIRECTION_TO_MAX;
      motor_set_direction(motor_direction);
    }
  }
}

void temp_update()
{
  if (millis() > temp_previous_time + temp_interval_update)
  {
    temp_previous_time = millis();

    temp_sensor.setWaitForConversion(false); // makes it async
    temp_sensor.requestTemperatures();
    temp_sensor.setWaitForConversion(true);

    uint8_t t = temp_sensor.getTempCByIndex(0);
    temp.set(t, !temp_selected_state);
  }
}

void humidity_update()
{
  if (millis() > humidity_previous_time + humidity_interval_update)
  {
    humidity_previous_time = millis();

    uint8_t h = humidity_sensor.readHumidity();
    humidity.set(h, !humidity_selected_state);
  }
}

void heater_update()
{
  if (not heater_state)
  {
    digitalWrite(HEATER_PIN, LOW);
    return;
  }

  if (millis() > heater_previous_time + heater_interval_update)
  {
    heater_previous_time = millis();

    uint8_t t = temp.get();

    if (t < temp_set)
    {
      digitalWrite(HEATER_PIN, HIGH);
    }
    else if (t > temp_set)
    {
      digitalWrite(HEATER_PIN, LOW);
    }
  }
}

void vapor_update()
{
  if (not vapor_state)
  {
    digitalWrite(VAPOR_PIN, LOW);
    return;
  }

  if (millis() > vapor_previous_time + vapor_interval_update)
  {
    vapor_previous_time = millis();

    uint8_t h = humidity.get();

    if (h < humidity_set)
    {
      digitalWrite(VAPOR_PIN, HIGH);
    }
    else if (h > humidity_set)
    {
      digitalWrite(VAPOR_PIN, LOW);
    }
  }
}

void fan_update()
{
  if (not fan_state)
  {
    digitalWrite(FAN_PIN, HIGH);
    return;
  }

  if (millis() > fan_previous_time + fan_interval_update)
  {
    fan_previous_time = millis();

    uint8_t h = humidity.get();

    if (h < humidity_set)
    {
      digitalWrite(FAN_PIN, HIGH);
    }
    else if (h > humidity_set)
    {
      digitalWrite(FAN_PIN, LOW);
    }
  }
}

void power_enable()
{
  move_open_button.disable();
  move_close_button.enable();
  heat_button.enable();
  light_button.enable();
  vapor_button.enable();
  fan_button.enable();

  t_up_button.enable();
  t_down_button.enable();
  h_up_button.enable();
  h_down_button.enable();

  hh_up_button.enable();
  hh_down_button.enable();
  mm_up_button.enable();
  mm_down_button.enable();

  change_color("t", TEXT_COLOR_DEFAULT);
  change_color("h", TEXT_COLOR_DEFAULT);
  // change_color("time", TEXT_COLOR_DEFAULT);
  change_color("t_Cels", TEXT_COLOR_DEFAULT);
  change_color("h_Perc", TEXT_COLOR_DEFAULT);

  timer_hours = read_eeprom_timer_hours_set();
  timer_minutes = read_eeprom_timer_minutes_set();
  timer_time = timer_hours * 60 + timer_minutes;
  change_text("time", format_two_digits(timer_hours) + ":" + format_two_digits(timer_minutes));
  
  heater_state = read_eeprom_button_temp_state();
  light_state = read_eeprom_button_light_state();
  fan_state = read_eeprom_button_cooler_state();
  vapor_state = read_eeprom_button_vapor_state();

  heat_button.set_state(heater_state);
  light_button.set_state(light_state);
  fan_button.set_state(fan_state);
  vapor_button.set_state(vapor_state);

  digitalWrite(HEATER_PIN, heater_state);
  digitalWrite(LIGHT_PIN, !light_state);
  digitalWrite(FAN_PIN, !fan_state);
  digitalWrite(VAPOR_PIN, vapor_state);
}

void power_disable()
{
  disable_all_objects();

  save_button_temp_state(heat_button.is_on());
  save_button_light_state(light_button.is_on());
  save_button_cooler_state(fan_button.is_on());
  save_button_vapor_state(vapor_button.is_on());

  // move_open_button.off();
  move_close_button.off();
  vapor_button.off();
  fan_button.off();
  heat_button.off();
  light_button.off();

  move_open_button.disable();
  heat_button.disable();
  light_button.disable();
  vapor_button.disable();
  fan_button.disable();

  t_up_button.disable();
  t_down_button.disable();
  h_up_button.disable();
  h_down_button.disable();

  hh_up_button.disable();
  hh_down_button.disable();
  mm_up_button.disable();
  mm_down_button.disable();

  change_color("t", TEXT_COLOR_DISABLED);
  change_color("h", TEXT_COLOR_DISABLED);
  change_color("time", TEXT_COLOR_DISABLED);
  change_color("t_Cels", TEXT_COLOR_DISABLED);
  change_color("h_Perc", TEXT_COLOR_DISABLED);

  timer_state = OFF;
  timer_selected_state = false;
  
  timer_time = 0;
  timer_hours = 0;
  timer_minutes = 0;
  change_text("time", format_two_digits(timer_hours) + ":" + format_two_digits(timer_minutes));

  light_flash_count = 0;
  buzzer_flash_count = 0;
}

void power(bool state)
{
  button_update(power_button, state);
  digitalWrite(LED_PIN, power_button.is_on());
  if (state == PRESSED)
  {
    if (power_button.is_on())
    {
      power_enable();
    }
    else
    {
      power_disable();
    }
  }
}

void move_open(bool state)
{
  button_update(move_open_button, state);
  if (move_open_button.is_on())
  {
    motor_on();
  }
  else
  {
    motor_off();
  }
}

void move_close(bool state)
{
  button_update(move_close_button, state);
  if (move_close_button.is_on())
  {
    motor_on();
  }
  else
  {
    motor_off();
  }
}

void heater(bool state)
{
  button_update(heat_button, state);
  heater_state = heat_button.is_on();
}

void vapor(bool state)
{
  button_update(vapor_button, state);
  vapor_state = vapor_button.is_on();
}

void light(bool state)
{
  button_update(light_button, state);
  digitalWrite(LIGHT_PIN, !light_button.is_on());

  light_flash_count = 0;
}

void fan(bool state)
{
  button_update(fan_button, state);
  fan_state = fan_button.is_on();
}

void timer(bool state)
{
  if (not power_button.is_on())
  {
    return;
  }

  if (timer_selected_state == true)
  {
    return;
  }

  if (state == PRESSED)
  {
    if (timer_time > 0)
    {
      timer_state = !timer_state;
      Serial.println("timer_state: " + String(timer_state));
    }
    

    if (timer_state == true)
    {
      change_color("time", TEXT_COLOR_DEFAULT);
    }
    else{
      change_text("time", format_two_digits(timer_hours) + ":" + format_two_digits(timer_minutes));
      change_color("time", TEXT_COLOR_DISABLED);
    }
  }
  
}

void temp_up(bool state)
{
  if (not t_up_button.is_enable())
  {
    return;
  }

  if (state == PRESSED)
  {
    if (temp_selected_state == true)
    {
      if (temp_set < TEMP_MAX)
      {
        temp_set++;
        save_temp_set(temp_set);
      }
    }
    change_color("t", TEXT_COLOR_SELECTED);
    change_text("t", String(temp_set));
    temp_selected_previous_time = millis();
    temp_selected_state = true;
  }
}

void humidity_up(bool state)
{
  if (not h_up_button.is_enable())
  {
    return;
  }

  if (state == PRESSED)
  {
    if (humidity_selected_state == true)
    {
      if (humidity_set < HUMIDITY_MAX)
      {
        humidity_set++;
        save_humidity_set(humidity_set);
      }
    }
    change_color("h", TEXT_COLOR_SELECTED);
    change_text("h", String(humidity_set));
    humidity_selected_previous_time = millis();
    humidity_selected_state = true;
  }
}

void temp_down(bool state)
{
  if (not t_down_button.is_enable())
  {
    return;
  }

  if (state == PRESSED)
  {
    if (temp_selected_state == true)
    {
      if (temp_set > TEMP_MIN)
      {
        temp_set--;
        save_temp_set(temp_set);
      }
    }
    change_color("t", TEXT_COLOR_SELECTED);
    change_text("t", String(temp_set));
    temp_selected_previous_time = millis();
    temp_selected_state = true;
  }
}

void humidity_down(bool state)
{
  if (not h_down_button.is_enable())
  {
    return;
  }

  if (state == PRESSED)
  {
    if (humidity_selected_state == true)
    {
      if (humidity_set > HUMIDITY_MIN)
      {
        humidity_set--;
        save_humidity_set(humidity_set);
      }
    }
    change_color("h", TEXT_COLOR_SELECTED);
    change_text("h", String(humidity_set));
    humidity_selected_previous_time = millis();
    humidity_selected_state = true;
  }
}

void hh_up(bool state)
{
  if (not hh_up_button.is_enable())
  {
    return;
  }

  if (state == PRESSED)
  {
    timer_state = ON;
    if (timer_hours < HH_MAX)
    {
      timer_hours++;
    }
    else
    {
      timer_hours = HH_MIN;
    }
    change_color("time", TEXT_COLOR_SELECTED);
    change_text("time", format_two_digits(timer_hours) + ":" + format_two_digits(timer_minutes));
    timer_selected_previous_time = millis();
    timer_selected_state = true;
    timer_need_save = true;
  }
}

void hh_down(bool state)
{
  if (not hh_down_button.is_enable())
  {
    return;
  }

  if (state == PRESSED)
  {
    timer_state = ON;
    if (timer_hours > HH_MIN)
    {
      timer_hours--;
    }
    else
    {
      timer_hours = HH_MAX;
    }
    change_color("time", TEXT_COLOR_SELECTED);
    change_text("time", format_two_digits(timer_hours) + ":" + format_two_digits(timer_minutes));
    timer_selected_previous_time = millis();
    timer_selected_state = true;
    timer_need_save = true;
  }
}

void mm_up(bool state)
{
  if (not mm_up_button.is_enable())
  {
    return;
  }

  if (state == PRESSED)
  {
    timer_state = ON;
    if (timer_minutes < MM_MAX)
    {
      timer_minutes++;
    }
    else
    {
      timer_minutes = MM_MIN;
    }
    change_color("time", TEXT_COLOR_SELECTED);
    change_text("time", format_two_digits(timer_hours) + ":" + format_two_digits(timer_minutes));
    timer_selected_previous_time = millis();
    timer_selected_state = true;
    timer_need_save = true;
  }
}

void mm_down(bool state)
{
  if (not mm_down_button.is_enable())
  {
    return;
  }

  if (state == PRESSED)
  {
    timer_state = ON;
    if (timer_minutes > MM_MIN)
    {
      timer_minutes--;
    }
    else
    {
      timer_minutes = MM_MAX;
    }
    change_color("time", TEXT_COLOR_SELECTED);
    change_text("time", format_two_digits(timer_hours) + ":" + format_two_digits(timer_minutes));
    timer_selected_previous_time = millis();
    timer_selected_state = true;
    timer_need_save = true;
  }
}

void timer_selected_update()
{
  if (not power_button.is_on())
  {
    return;
  }

  if (timer_selected_state == false)
  {
    return;
  }

  if (millis() > timer_selected_previous_time + timer_selected_timer)
  {
    change_color("time", TEXT_COLOR_DEFAULT);
    timer_selected_state = false;

    if (timer_need_save)
    {
      save_timer_set(timer_hours, timer_minutes);
      timer_need_save = false;
    }
  }
}

void temp_selected_update()
{
  if (not power_button.is_on())
  {
    temp_selected_state = false;
    return;
  }

  if (temp_selected_state == false)
  {
    return;
  }

  if (millis() > temp_selected_previous_time + temp_selected_timer)
  {
    temp.set(temp_sensor.getTempCByIndex(0));
    change_color("t", TEXT_COLOR_DEFAULT);
    temp_selected_state = false;
  }
}

void humidity_selected_update()
{
  if (not power_button.is_on())
  {
    humidity_selected_state = false;
    return;
  }

  if (humidity_selected_state == false)
  {
    return;
  }

  if (millis() > humidity_selected_previous_time + humidity_selected_timer)
  {
    humidity.set(humidity_sensor.readHumidity());
    change_color("h", TEXT_COLOR_DEFAULT);
    humidity_selected_state = false;
  }
}

void ligh_update()
{
  if (not power_button.is_on())
  {
    return;
  }

  if (light_flash_count > 0)
  {
    if (millis() > light_previous_time + light_flash_interval)
    {
      light_previous_time = millis();
      
      digitalWrite(LIGHT_PIN, !digitalRead(LIGHT_PIN));

      light_flash_count--;
    }
  }
}

void buzzer_update()
{
  if (not power_button.is_on())
  {
    return;
  }

  if (buzzer_flash_count > 0)
  {
    if (millis() > buzzer_previous_time + buzzer_flash_interval)
    {
      buzzer_previous_time = millis();
      
      digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));

      buzzer_flash_count--;
    }
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void colon_update()
{
  if (timer_state == OFF)
  {
    return;
  }

  if (timer_selected_state == true)
  {
    return;
  }
  
  if (millis() > colon_previous_time + colon_interval_update)
  {
    colon_previous_time = millis();

    if (colon == ":")
    {
      colon = " ";
    }
    else
    {
      colon = ":";
    }

    change_text("time", format_two_digits(timer_hours) + colon + format_two_digits(timer_minutes));
  }
}

void timer_update()
{
  if (timer_state == OFF)
  {
    return;
  }

  if (millis() > timer_previous_time + timer_interval_update)
  {
    timer_previous_time = millis();
    timer_time = timer_hours * 60 + timer_minutes;

    if (timer_time > 0)
    {
      if (timer_selected_state == false)
      {
        if (timer_minutes > 0)
        {
          timer_minutes--;
        }
        else{
          timer_minutes = 59;
          timer_hours--;
        }

        change_text("time", format_two_digits(timer_hours) + colon + format_two_digits(timer_minutes));
      }
    }
    
    timer_time = timer_hours * 60 + timer_minutes;
    
    if (timer_time == 0)
    {
      change_text("time", format_two_digits(timer_hours) + ":" + format_two_digits(timer_minutes));
      change_color("time", TEXT_COLOR_DISABLED);
      timer_state = OFF;
      light_flash_count = LIGHT_FLASH_TIMES * 2;
      buzzer_flash_count = BUZZER_FLASH_TIMES * 2;
    }
  }
}

void display_update()
{
  bool state = RELEASED;
  uint8_t button = 0;

  if (read_nextion(button, state))
  {
    Serial.print(button);
    Serial.print(" ");
    Serial.print(state);
    Serial.print(" ");
    Serial.println("OK");

    switch (button)
    {
    case 1: // power
      power(state);
      break;

    case 2: // move open
      move_open(state);
      break;

    case 3: // heater
      heater(state);
      break;

    case 4: // light
      light(state);
      break;

    case 5: // vapor
      vapor(state);
      break;

    case 6: // fan
      fan(state);
      break;

    case 7: // timer
      timer(state);
      break;

    case 10: // temp up
      temp_up(state);
      break;

    case 11: // humidity up
      humidity_up(state);
      break;

    case 12: // temp down
      temp_down(state);
      break;

    case 13: // humidity down
      humidity_down(state);
      break;

    case 14: // hh up
      hh_up(state);
      break;

    case 15: // hh down
      hh_down(state);
      break;

    case 16: // mm up 
      mm_up(state);
      break;

    case 17: // mm down
      mm_down(state);
      break;

    case 20: // move close
      move_close(state);
      break;

    default:
      break;
    }
  }
}

void setup()
{
  disable_all_objects();
  init_all_pins();

  Serial.begin(9600);
  Serial2.begin(9600);

  digitalWrite(LED_PIN, HIGH);
  delay(5000);
  digitalWrite(LED_PIN, LOW);

  change_to_page1();
 
  buttons_init();

  eeprom_settings_init();
  temp_set = read_eeprom_temp_set();
  humidity_set = read_eeprom_humidity_set();

  timer_hours = read_eeprom_timer_hours_set();
  timer_minutes = read_eeprom_timer_minutes_set();
  timer_time = timer_hours * 60 + timer_minutes;
  // change_text("time", format_two_digits(timer_hours) + ":" + format_two_digits(timer_minutes));

  temp_sensor.begin();
  temp_sensor.setResolution(9);

  humidity_sensor.begin();
}

void loop()
{
  endstops_update();
  motor_update();

  temp_update();
  heater_update();

  humidity_update();
  vapor_update();
  fan_update();

  timer_update();

  display_update();
  temp_selected_update();
  humidity_selected_update();
  timer_selected_update();
  colon_update();

  ligh_update();
  buzzer_update();
}
#include "Arduino.h"
#include "images.h"
#include "nextion.h"
#include "buttons.h"
#include "common.h"
#include "physical_quantity.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "DHT.h"


#define LED_PIN 2
#define LIGHT_PIN 21
#define FAN_PIN 19
#define MOTOR_ENABLE_PIN 14
#define MOTOR_A_PIN 18
#define MOTOR_B_PIN 5
#define VAPOR_PIN 27
#define HEATER_PIN 26
#define ENDSTOP_PIN 34
#define TEMP_SENSOR_PIN 32
#define HUMIDITY_SENSOR_PIN 33

#define TEMP_MIN 10 // 50
#define TEMP_MAX 80
#define HUMIDITY_MIN 10 // 40
#define HUMIDITY_MAX 80

#define DHTTYPE DHT21 // DHT 21 (AM2301)

#define TEXT_COLOR_DEFAULT 63320
#define TEXT_COLOR_SELECTED 1631


OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature temp_sensor(&oneWire);

DHT humidity_sensor(HUMIDITY_SENSOR_PIN, DHTTYPE);

bool button1_state = OFF;
bool endstop_state = OPEN;
bool motor_enable = OFF;
bool motor_direction = CW;

uint32_t endstop_previous_time = 0;
uint16_t endstop_interval_update = 50; // ms

uint32_t temp_previous_time = 0;
uint16_t temp_interval_update = 2000; // ms

uint32_t humidity_previous_time = 0;
uint16_t humidity_interval_update = 2000; // ms

uint32_t heater_previous_time = 0;
uint16_t heater_interval_update = 1000; // ms

LatchingButton power_button = LatchingButton("power");
LatchingButton move_button = LatchingButton("move");

LatchingButton vapor_button = LatchingButton("vap");
LatchingButton fan_button = LatchingButton("fan");
LatchingButton heat_button = LatchingButton("heat");
LatchingButton light_button = LatchingButton("light");

Button t_up_button = Button("t_up");
Button t_down_button = Button("t_down");
Button h_up_button = Button("h_up");
Button h_down_button = Button("h_down");

PhysicalQuantity temp = PhysicalQuantity("t", TEMP_MIN, TEMP_MAX);
PhysicalQuantity humidity = PhysicalQuantity("h", HUMIDITY_MIN, HUMIDITY_MAX);

uint8_t temp_set = 0;
uint8_t humidity_set = 0;

void buttons_init(){
  power_button.set_pics(big_on_normal, big_on_pressed, big_off_normal, big_off_pressed, big_disable_normal);
  move_button.set_pics(big_on_normal, big_on_pressed, big_off_normal, big_off_pressed, big_disable_normal);
  
  vapor_button.set_pics(small_on_normal, small_on_pressed, small_off_normal, small_off_pressed, small_disable_normal);
  fan_button.set_pics(small_on_normal, small_on_pressed, small_off_normal, small_off_pressed, small_disable_normal);
  heat_button.set_pics(small_on_normal, small_on_pressed, small_off_normal, small_off_pressed, small_disable_normal);
  light_button.set_pics(small_on_normal, small_on_pressed, small_off_normal, small_off_pressed, small_disable_normal);

  t_up_button.set_pics(up_normal, up_pressed, up_disable);
  t_down_button.set_pics(down_normal, down_pressed, down_disable);
  h_up_button.set_pics(up_normal, up_pressed, up_disable);
  h_down_button.set_pics(down_normal, down_pressed, down_disable);

  power_button.init(ENABLE);
  move_button.init(DISABLE);
  
  vapor_button.init(DISABLE);
  fan_button.init(DISABLE);
  heat_button.init(DISABLE);
  light_button.init(DISABLE);

  t_up_button.init(DISABLE);
  t_down_button.init(DISABLE);
  h_up_button.init(DISABLE);
  h_down_button.init(DISABLE);
}

void button_update(LatchingButton &button, bool state){
  if (state == PRESSED){
    if (button.is_enable()){
      button.inverse_on_off_state();
    }
  }

  button.set_press_state(state);
}

void endstop_update(){
  if (millis() > endstop_previous_time + endstop_interval_update){
    endstop_previous_time = millis();

    bool state = digitalRead(ENDSTOP_PIN);
    
    if (state != endstop_state){
      endstop_state = state;
      Serial.println(endstop_state);
    }
  }
}

void motor_on(){
  Serial.println("motor_on");
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
}

void motor_off(){
  Serial.println("motor_off");
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
}

void motor_set_ditection(bool direction){
  if (direction == CW){
    digitalWrite(MOTOR_A_PIN, HIGH);
    digitalWrite(MOTOR_B_PIN, LOW);
  }
  else{
    digitalWrite(MOTOR_A_PIN, LOW);
    digitalWrite(MOTOR_B_PIN, HIGH);
  }
}

void motor_update(){
  if (endstop_state == true){
    motor_off();
    motor_set_ditection(motor_direction);
  }
}

void temp_update(){
  if (millis() > temp_previous_time + temp_interval_update){
    temp_previous_time = millis();
    
    temp_sensor.setWaitForConversion(false);  // makes it async
    temp_sensor.requestTemperatures();
    temp_sensor.setWaitForConversion(true);
    
    temp.set(temp_sensor.getTempCByIndex(0));
  }
}

void humidity_update(){
  if (millis() > humidity_previous_time + humidity_interval_update){
    humidity_previous_time = millis();
    
    uint8_t h = humidity_sensor.readHumidity();
    
    humidity.set(h);
  }
}

void heater_update(){
  if (millis() > heater_previous_time + heater_interval_update){
    heater_previous_time = millis();
    
    uint8_t t = temp.get();

    if (t < temp_set){
      digitalWrite(HEATER_PIN, HIGH);
    }
    else if (t > temp_set){
      digitalWrite(HEATER_PIN, LOW);
    }
  }
}

void display_update(){
  bool state = RELEASED;
  uint8_t button = 0;

  if (read_nextion(button, state)) {
    Serial.print(button);
    Serial.print(" ");
    Serial.print(state);
    Serial.print(" ");
    Serial.println("OK");

    switch (button){
      case 1:
        button_update(power_button, state);
        digitalWrite(LED_PIN, power_button.is_on());
        if (state == PRESSED){
          if (power_button.is_on()){
            move_button.enable();
            heat_button.enable();
            light_button.enable();
            vapor_button.enable();
            fan_button.enable();
            t_up_button.enable();
            t_down_button.enable();
            h_up_button.enable();
            h_down_button.enable();
          }
          else{
            move_button.disable();
            heat_button.disable();
            light_button.disable();
            vapor_button.disable();
            fan_button.disable();
            t_up_button.disable();
            t_down_button.disable();
            h_up_button.disable();
            h_down_button.disable();
          }
        }
        break;

      case 2:
        button_update(move_button, state);
        if (move_button.is_on()){
          motor_on();
        }
        else{
          motor_off();
        }
        break;

      case 3:
        button_update(heat_button, state);
        digitalWrite(HEATER_PIN, heat_button.is_on());
        break;

      case 4:
        button_update(light_button, state);
        digitalWrite(LIGHT_PIN, !light_button.is_on());
        break;

      case 5:
        button_update(vapor_button, state);
        digitalWrite(VAPOR_PIN, !vapor_button.is_on());
        break;

      case 6:
        button_update(fan_button, state);
        digitalWrite(FAN_PIN, !fan_button.is_on());
        break;

      case 10: // temp up
        if (state == PRESSED){
          temp.set(temp.get() + 1);
          change_color("t", TEXT_COLOR_SELECTED);
        }
        break;

      case 11: // humidity up
        if (state == PRESSED){
          humidity.set(humidity.get() + 1);
        }
        break;
      
      case 12: // temp down
        if (state == PRESSED){
          temp.set(temp.get() - 1);
          change_color("t", TEXT_COLOR_DEFAULT);
        }
        break;
      
      case 13: // humidity down
        if (state == PRESSED){
          humidity.set(humidity.get() - 1);
        }
        break;
      
      default:
        break;
    }
  }
}

void setup() {
  digitalWrite(LIGHT_PIN, HIGH);
  digitalWrite(FAN_PIN, HIGH);
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
  digitalWrite(MOTOR_A_PIN, HIGH);
  digitalWrite(MOTOR_B_PIN, LOW);
  digitalWrite(VAPOR_PIN, HIGH);
  digitalWrite(HEATER_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_A_PIN, OUTPUT);
  pinMode(MOTOR_B_PIN, OUTPUT);
  pinMode(VAPOR_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(TEMP_SENSOR_PIN, INPUT_PULLUP);
  pinMode(HUMIDITY_SENSOR_PIN, INPUT_PULLUP);

  pinMode(ENDSTOP_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  Serial2.begin(9600);

  buttons_init();

  temp_set = 27;
  humidity_set = 75;

  temp_sensor.begin();
  temp_sensor.setResolution(9);

  humidity_sensor.begin();

  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}

void loop() {
  endstop_update();
  temp_update();
  heater_update();
  humidity_update();
  // motor_update();

  display_update();
}
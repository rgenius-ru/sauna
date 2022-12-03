#include "nextion.h"


bool read_nextion(uint8_t &obj, bool &state) {
  if (Serial2.available() == 0) {
    return false;
  }

  uint8_t count = 0;

  while (Serial2.available() > 0) {
    uint8_t byte_read = Serial2.read();

    // if (count == 3 or count == 4 or count == 5){
    //   Serial.print(" ");
    // }

    if (byte_read <= 0x0F) {
      // Serial.print("0");
    }

    if (count == 2) {
      obj = byte_read;
      // Serial.print(byte_read, HEX);
    }

    if (count == 3) {
      state = bool(byte_read);
      // Serial.print(byte_read, HEX);
    }

    // Serial.print(byte_read, HEX);

    count++;
  }

  // Serial.println(comm);

  if (count < 7) {
    return false;
  }

  return true;
}

void write_nextion(String command) {
  Serial2.print(command);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
}

void change_page_to(uint8_t page_id)
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
    Serial2.print("page " + String(page_id));
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

    if (str == "page" + String(page_id) + " initialized")
    {
      break;
    }

    Serial.println();
  }
}

void change_pic(String obj_name, uint8_t id_pic) {
  String command = obj_name + "=" + String(id_pic);
  write_nextion(command);
  Serial.println(command);
}

void change_text(String obj_name, String text){
  String _text = obj_name + ".txt=" + "\"" + text + "\"";
  write_nextion(_text);
  Serial.println(_text);
}

void change_color(String obj_name, uint32_t color){
  String _text = obj_name + ".pco=" + String(color);
  write_nextion(_text);
  Serial.println(_text);
}

void error_show(String error_text){
  String obj_name = "top_text";
  // uint32_t color = 65535;
  // uint32_t height = 110;

  String _text = "";

  change_page_to(ERROR_PAGE_ID);

  // _text = obj_name + ".hig=" + String(height);
  // write_nextion(_text);
  // Serial.println(_text);
  
  // _text = obj_name + ".bco=" + String(color);
  // write_nextion(_text);
  // Serial.println(_text);

  _text = obj_name + ".txt=" + "\"" + error_text + "\"";
  write_nextion(_text);
  Serial.println(_text);
}

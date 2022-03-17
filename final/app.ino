#include <RFID.h>
#include <Servo.h>
#include <Led.h>
#include <Buzzer.h>
#include <MiniCom.h>
#include <Button.h>

RFID rfid;
Buzzer buzzer(8);
MiniCom com;
Led red_led(5);
Led blue_led(4);
Servo servo;
Button btn(2);

byte rfidId[4] = {0, 0, 0, 0};

bool registering = false; // 등록된 id가 있고 rfid가 읽는 상황 && registering 이 아닌 상황
bool id_existing = false; // 나중에 EEPROM 에 있으면 initstate에서 id_existing = true로 바꿔주자
bool card_attaching = false;

void close_door()
{
  blue_led.off();
  red_led.on();
  buzzer.beep(300);
  com.print(0, "Wrong");
  com.print(1, "Retry...");
  delay(2000);
  red_led.off();
  com.print(0, "Tag");
  com.print(1, "Your RFID");
}

void open_door()
{
  servo.write(180);
  blue_led.on();
  red_led.off();
  com.print(0, "Correct");
  com.print(1, "Door Open");
  delay(5000);
  blue_led.off();
  servo.write(0);
  com.print(0, "Tag");
  com.print(1, "Your RFID");
}

void register_success(byte *id1, byte *id2)
{
  for (int i = 0; i < 4; i++)
  {
    EEPROM.write(i, id1[i]);
    id1[i] = id2[i];
  }
  id_existing = true;
  registering = false;
}

void setup()
{
  Serial.begin(115200);
  rfid.init();
  rfid.setCallback(open_door, close_door);
}

void loop()
{
  rfid.check();
}
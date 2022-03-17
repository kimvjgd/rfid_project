#include <SPI.h>     //
#include <MFRC522.h> //
#include <EEPROM.h>  //
#include <MiniCom.h> //
#include <Button.h>  //
#include <Servo.h>   //
#include <Led.h>     //
#include <Buzzer.h>  //

const int RST_PIN = 9;         //
const int SS_PIN = 10;         //
MFRC522 mfrc(SS_PIN, RST_PIN); //

MiniCom com;   //
Button btn(2); //

byte rfidId[4] = {0, 0, 0, 0}; //

Buzzer buzzer(8); //
Led red_led(5);   //
Led blue_led(4);  //
Servo servo;      //

bool registering = false;    // 등록된 id가 있고 rfid가 읽는 상황 && registering 이 아닌 상황            //
bool id_existing = false;    // 나중에 EEPROM 에 있으면 initstate에서 id_existing = true로 바꿔주자            //
bool card_attaching = false; //

bool equalId(byte *id1, byte *id2) //
{                                  //
  for (byte i = 0; i < 4; i++)     //
  {                                //
    if (id1[i] != id2[i])          //
    {                              //
      return false;                //
    }                              //
  }                                //
  return true;                     //
} //

void register_success(byte *id1, byte *id2) //
{                                           //
  for (int i = 0; i < 4; i++)               //
  {                                         //
    EEPROM.write(i, id1[i]);                //
    id1[i] = id2[i];                        //
  }                                         //
  id_existing = true;                       //
  registering = false;                      //
} //
// 여기까지는 문제 없는 것 같다.

void register_id()
{
  registering = true;

  int init_time = millis();
  if (id_existing == false)
  {
    com.print(0, "Tag for First");
    com.print(1, "Registration");
    while (registering == true)
    {
      int time_lapse = millis();
      if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) // 태그 접촉이 안될때
      {
      }
      else
      {
        register_success(rfidId, mfrc.uid.uidByte);

        delay(1000);
      }
      if (time_lapse - init_time > 5000)
      {
        com.print(0, "Tag");
        com.print(1, "Your RFID");
        if (id_existing == false)
        {
          com.print(0, "PLEASE REGISTER");
          com.print(1, "NO ID, NOW");
        }
        break;
      }
    }
  }
  else
  {
    com.print(0, "Change");
    com.print(1, "Your Id");
    while (registering == true)
    {
      int time_lapse = millis();
      if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) // 태그 접촉이 안될때
      {
      }
      else
      {
        register_success(rfidId, mfrc.uid.uidByte);
        delay(1000);
      }
      if (time_lapse - init_time > 5000)
      {
        com.print(0, "Tag");
        com.print(1, "Your RFID");
        if (id_existing == false)
        {
          com.print(0, "PLEASE REGISTER");
          com.print(1, "NO ID, NOW");
        }
        break;
      }
    }
  }

  for (int i = 0; i < 4; i++)   //
  {                             //
    EEPROM.write(i, rfidId[i]); //
  }                             //
}

void close_door()            //
{                            //
  blue_led.off();            //
  red_led.on();              //
  buzzer.beep(300);          //
  com.print(0, "Wrong");     //
  com.print(1, "Retry...");  //
  delay(2000);               //
  red_led.off();             //
  servo.write(0);            //
  com.print(0, "Tag");       //
  com.print(1, "Your RFID"); //
} //

void open_door()             //
{                            //
  servo.write(180);          //
  blue_led.on();             //
  red_led.off();             //
  com.print(0, "Correct");   //
  com.print(1, "Door Open"); //
  delay(5000);               //
  blue_led.off();            //
  servo.write(0);            //
  com.print(0, "Tag");       //
  com.print(1, "Your RFID"); //
} //

void check_id()
{
  if (id_existing == true && registering == false)
  {

    if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial())
    {
      return;
    }
    buzzer.beep(50); //

    if (equalId(rfidId, mfrc.uid.uidByte) == true)
    {
      open_door();
    }
    else
    {
      close_door();
    }
  }
}

void setup()
{
  servo.attach(7);
  servo.write(0);

  btn.setCallback(register_id);
  SPI.begin();
  mfrc.PCD_Init();

  com.setInterval(1000, check_id);
  com.init();
  com.print(0, "PLEASE REGISTER");
  com.print(1, "NO ID, NOW");

  for (int i = 0; i < 4; i++)
  {
    rfidId[i] = EEPROM.read(i);
  }
  if (EEPROM.read(0) != 0 && EEPROM.read(1) != 0 && EEPROM.read(2) != 0 && EEPROM.read(3) != 0)
  {
    id_existing = true;
    com.print(0, "Tag");
    com.print(1, "Your RFID");
  }
}

void loop()
{
  com.run();
  btn.check();

  if (id_existing == false)
  {
    com.print(0, "TAG FOR");
    com.print(1, "REGISTRATION");
  }
}

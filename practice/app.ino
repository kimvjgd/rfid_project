// 수  행  평  가

/*
다음 조건을 만족하는 RFID 기반 출입문 시스템을 만드세요.
1) 구성: RFID 리더기/태그, 서보모터, 부저, 녹색 LED, 적색 LED, 버튼
2) 버튼을 누르면 5초 안에 태그의 RFID 값을 EEPROM에 저장
3) 초기화 될 때 EEPROM에 저장된 값을 읽어 출입 가능 태그값으로 처리
4) 태그를 읽었을 때 등록된 태그이면 서보 모터로 문을 열어줌
5) 열린 문은 5초 후 자동으로 닫힘
6) 태그를 읽을 때 짧게 비프음과 녹색 LED를 켰다 끔
7) 등록된 태그가 아닌 경우 2초 동안 적색 LED를 켜고 비프음 울림 
8) RFIDReader 클래스를 제작해서 제어
9) 필요하다 생각하는 정보는 LCD로 출력
10) 제출물:
  - 새로 만든 라이브러리 클래스(기존 클래스는 제외)
  - 회로 구성 촬영 사진(jpg, 파일크기 2M이내)
  - 메인 app.ino 파일
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Button.h>
#include <MiniCom.h>
#include <Servo.h>
#include <Led.h>

const int RST_PIN = 9;
const int SS_PIN = 10;

Led red_led(5);
Led blue_led(4);

Servo servo;

MiniCom com;

Button btn(2);
SimpleTimer timer;

MFRC522 mfrc(SS_PIN, RST_PIN);

// 나의 rfid 카드 id
byte myId[4] = {};

bool registering = false; // 등록된 id가 있고 rfid가 읽는 상황 && registering 이 아닌 상황
bool id_existing = false; // 나중에 EEPROM 에 있으면 initstate에서 id_existing = true로 바꿔주자
bool card_attaching = false;

bool equalId(byte *id1, byte *id2)
{
  for (byte i = 0; i < 4; i++)
  {
    if (id1[i] != id2[i])
    {
      return false;
    }
  }
  return true;
}

void register_success(byte *id1, byte *id2)
{
  for (int i = 0; i < 4; i++)
  {
    id1[i] = id2[i];
  }
  id_existing = true;
  registering = false;
}

void register_id()
{
  registering = true;
  SimpleTimer timer;

  if (id_existing == false)
  {
    while (registering == true)
    {
      if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) // 태그 접촉이 안될때
      {
        Serial.println("No attached");
      }
      else
      {
        register_success(myId, mfrc.uid.uidByte);
        com.print(0, "Id");
        com.print(0, "Registered");
        delay(1000);
      }
    }
  }
  else
  {
    while (registering == true)
    {
      if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) // 태그 접촉이 안될때
      {
        Serial.println("No attached");
      }
      else
      {

        register_success(myId, mfrc.uid.uidByte);
        Serial.println("Re - ");
        delay(1000);
      }
    }
  }
}

void close_door()
{
  blue_led.off();
  red_led.on();
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

void check_id()
{
  if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial())
  {
    // 태그 접촉이 되지 않았을 때 또는 ID가 읽혀지지 않았을 때
    // delay(300);
    Serial.println("sssss");
    return;
  }

  Serial.print("mfrc.uid.uidByte : ");
  for (byte i = 0; i < 4; i++)
  {
    Serial.print(mfrc.uid.uidByte[i]);
    Serial.print(" ");
  }
  Serial.println();

  if (equalId(myId, mfrc.uid.uidByte) == true)
  {

    open_door();
  }
  else
  {
    Serial.print("Not Equal");
    servo.write(0);

    close_door();
  }
}

void setup()
{
  servo.attach(7);
  btn.setCallback(register_id);
  Serial.begin(115200);
  SPI.begin();
  mfrc.PCD_Init();

  // timer.setInterval(1000, check_id);
  com.init();
  com.print(0, "Register");
  com.print(1, "First");
}

void loop()
{
  com.run();
  btn.check();

  Serial.print("myId : ");
  for (byte i = 0; i < 4; i++)
  {
    Serial.print(myId[i]);
    Serial.print(" ");
  }
  Serial.println();

  if (id_existing == true && registering == false)
  {

    check_id();
  }
}

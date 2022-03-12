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
#include <Servo.h>

const int RST_PIN = 9;
const int SS_PIN = 10;

MFRC522 mfrc(SS_PIN, RST_PIN);

Servo servo;

// 나의 rfid 카드 id
byte myId[] = {83, 2, 127, 49};

void setup()
{
  Serial.begin(115200);
  SPI.begin();
  mfrc.PCD_Init();
  servo.attach(7);
}

void loop()
{
  if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial())
  {
    // 태그 접촉이 되지 않았을 때 또는 ID가 읽혀지지 않았을 때
    delay(500);
    return;
  }
  Serial.print("Card UID:"); // 태그의 ID출력
  // 태그의 ID출력하는 반복문.태그의 ID사이즈(4)까지
  for (byte i = 0; i < 4; i++)
  {
    // mfrc.uid.uidByte[0] ~ mfrc.uid.uidByte[3]까지 출력
    Serial.print(mfrc.uid.uidByte[i]);
    Serial.print(" "); // id 사이의 간격 출력
  }
  Serial.println();
  if (equalId(myId, mfrc.uid.uidByte) == true)
  {
    Serial.print("Equal");
    servo.write(180);
  }
  else
  {
    Serial.print("Not Equal");
    servo.write(0);
  }
  // 여기 다시 보자.... 부져부분 넘어갔다 . 3 / 7 5:00~ 시작쪽
}

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
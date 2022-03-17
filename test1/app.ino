#include <RFID.h>
#include <Buzzer.h>

Buzzer buzzer(8);

RFID rfid;

void open_door()
{
  Serial.println("Equal");
  buzzer.beep(50);
}

void close_door()
{
  Serial.println("Not Equal");
  buzzer.beep(1500);
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
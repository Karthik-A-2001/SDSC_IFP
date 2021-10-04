
#include <SoftwareSerial.h>
#include <TinyGPS.h>

SoftwareSerial gsm(2, 3);
TinyGPS gps;

char val;
float gpslat, gpslon;
short rec = -1;
int buttonState = 0;
int led = 13;
const int buzzer = 11;

void setup()
{
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  gsm.begin(9600);
  Serial.begin(9600);
  gsm.println("AT");
  gsm.println("AT+CMGF=1");
  gsm.println("AT+CNMI=2,2,0,0,0");
}

void loop()
{
  digitalWrite(buzzer, HIGH);
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1023.0);

  while (Serial.available())
  {
    int c = Serial.read();
    if (gps.encode(c))
    {
      gps.f_get_position(&gpslat, &gpslon);
    }
  }
  String msg;
  while (gsm.available() > 0)
  {
    val = gsm.read();
    msg.concat(val);
    rec = msg.indexOf("Get location");

  }
  if (rec != -1)
  {
    rec = -1;
    SendMessage();
  }
  else if (voltage > 4 )
  {
    digitalWrite(led, HIGH);
    digitalWrite(buzzer, LOW);
    MakeCall();
    SendMessage();

  }
  else
  {
    digitalWrite(led, LOW);
    digitalWrite(buzzer, HIGH);
    HangupCall();

  }
}

void SendMessage()
{
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println("AT+CMGS=\"+91**********\"\r");//Enter num
  delay(1000);
  gsm.print("https://www.google.com/maps/?q=");
  gsm.print(gpslat, 6);
  gsm.print(",");
  gsm.print(gpslon, 6);
  delay(100);
  gsm.println((char)26);
  delay(1000);
}

void MakeCall()
{
  gsm.println("ATD+91**********;");
  delay(1000);
}

void HangupCall()
{
  gsm.println("ATH");
  delay(1000);
}

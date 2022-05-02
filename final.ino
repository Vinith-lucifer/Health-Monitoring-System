#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000

PulseOximeter pox;

uint32_t tsLastReport = 0;

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

#define DHTPIN D7   
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char auth[] = "49LekHNr8qM1nGfACxxdlJi14R9weqN-";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Smart";
char pass[] = "12345678";

BlynkTimer timer;

int heartrate;
int SP;

void setup()
{

  Serial.begin(9600);
lcd.init();
 lcd.backlight();
   dht.begin();
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("connecting to");
      lcd.setCursor(0,1);
   lcd.print(ssid);
 Blynk.begin(auth, ssid, pass);
    lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("connected to");
      lcd.setCursor(0,1);
   lcd.print(ssid);
   
  timer.setInterval(100,pul);
  timer.setInterval(3000,temp);
  timer.setInterval(5000,heart);
  Serial.print("Initializing pulse oximeter..");
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    pox.setOnBeatDetectedCallback(onBeatDetected);

}

void onBeatDetected()
{
    Serial.println("Beat!");
}

void loop()
{
  Blynk.run();
  timer.run();
}


void pul()
{
  pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");
        heartrate = pox.getHeartRate();
        SP = pox.getSpO2();
        Blynk.virtualWrite(V7,pox.getHeartRate());
        Blynk.virtualWrite(V8,pox.getSpO2());

  

        tsLastReport = millis();
    }

       
}


void temp()
{
  float t = dht.readTemperature();
   Blynk.virtualWrite(V9, t);
     lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("HR:" + String(pox.getHeartRate()) + " Spo2:" + String(pox.getSpO2()));
      lcd.setCursor(0,1);
   lcd.print("Temp:" + String(t));

   
}

void heart()
{
  if (heartrate>90)
        {
          Blynk.notify("Something is wrong");
          Blynk.virtualWrite(V12,"Something is wrong");
          
        }

        else if (heartrate<60)
        {
          Blynk.notify("Something is wrong");
          Blynk.virtualWrite(V12,"Something is wrong");
        }
        else
        {
          Blynk.virtualWrite(V12,"normal");
          Serial.println("normal");
        }

  if (SP<90)
  {
    Blynk.virtualWrite(V13,"oxygen level low");
  }
  else
  {
   Blynk.virtualWrite(V13,"oxygen level normal");
  }
  
}

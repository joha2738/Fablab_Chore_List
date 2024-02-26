/*
WIFI                virker
Hent tid            virker
Skift på kontakter  virker ikke

*/

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

// Wi-Fi netværksoplysninger
const char *ssid = "newdahl";
const char *password = "12345678";
String hostname = "Fablab Chore List";

// NTP Client opsætning
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);

// LED strip opsætning
#define LED_PIN   14  // Erstat med den GPIO pin hvor LED strip er tilsluttet, hvis nødvendigt
#define LED_COUNT 15
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Kontakternes pins (skal ændres til de faktiske GPIO pins du bruger)
// Pin  K Farve   Kontakt
//  2   2 Grå2    1
//  4   2 Lilla2  2
//  5   2 Blå     3
//  18  2 Grøn    4
//  19  2 Gul     5
//  21  2 Orange  6
//  22  2 Rød     7
//  23  2 Brun    8
//  25  2 Sort    9
//  26  2 Hvid    10
//  27  2 Grå1    11
//
//  32  1 Sort2   12
//  33  1 Hvid2   13
//  34  1 Grå     14
//  35  1 Lilla   15


const int switchPins[15] = {2, 4, 5, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35}; // Eksempel på GPIO numre
bool lastSwitchStates[15] = {false};



void setup() {
//Start seriel forbindelse
Serial.begin(115200);

// Forbinder til Wi-Fi
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str()); //define hostname
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Starter NTP Client
  timeClient.begin();

  // Initialiserer LED strip
  strip.begin();
  strip.show();

 // Initialiserer kontakternes pins som input
for (int i = 0; i < 15; i++) {
    pinMode(switchPins[i], INPUT_PULLUP); // Brug INPUT_PULLUP kontakter er koblet til GND
    lastSwitchStates[i] = digitalRead(switchPins[i]);
}

}

void loop() {
  //Hent aktuel tid
  timeClient.update();
  unsigned long currentEpochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&currentEpochTime);

  //Aflæs hentet id og sæt hour til timetallet
  int hour = ptm->tm_hour;
  int minute = ptm->tm_min;

  //Skift til LED'erne til rød vil kun ske når timerne er 7 om minutterne 00
  static bool updatedRed = false;
  if (hour == 7 && minute == 0 && !updatedRed) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0)); // Rød
    }
    strip.show();
    updatedRed = true;
  } else if (hour != 7 || minute != 0) {
    updatedRed = false;
  }

  // Tænder alle LED'er røde kl. 7
  if (hour == 7 && minute == 0) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0)); // Rød
    }
    strip.show();
  }

// Kontrollerer om kontakterne har skiftet posistion og ændre den tilhørende LED's farve
  for (int i = 0; i < 15; i++) {
    bool currentSwitchState = digitalRead(switchPins[i]);
    int ledIndex = i == 0 ? LED_COUNT - 1 : i - 1; // Mapper første kontakt til sidste LED, resten 1-til-1

    if (currentSwitchState != lastSwitchStates[i] && currentSwitchState == HIGH) {
      lastSwitchStates[i] = currentSwitchState;
      if (!(hour == 7 && minute == 0)) {
        strip.setPixelColor(ledIndex, strip.Color(0, 255, 0)); // Grøn
        strip.show();
      }
    } else {
      lastSwitchStates[i] = currentSwitchState;
    }
  }

  delay(1000);

 /* 
 
 Denne del virker ikke!!!!
 
 // Tjekker hver kontakt og opdaterer LED'ernes farve
  for (int i = 0; i < 15; i++) {
    bool currentSwitchState = digitalRead(switchPins[i]);
    if (currentSwitchState != lastSwitchStates[i]) {
      lastSwitchStates[i] = currentSwitchState;
      if (currentSwitchState) {
        // Hvis kontakten er skiftet til ON, og det ikke er kl. 7, skift til grøn
        if (hour != 7) {
          strip.setPixelColor(15-i, strip.Color(0, 255, 0)); // Grøn
          strip.show();
        }
      }
    }
  }*/
}

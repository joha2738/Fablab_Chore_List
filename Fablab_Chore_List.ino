#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

// Wi-Fi netværksoplysninger
const char *ssid = "newdahl";
const char *password = "12345678";

// NTP Client opsætning
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);

// LED strip opsætning
#define LED_PIN   14  // Erstat med den GPIO pin hvor LED strip er tilsluttet, hvis nødvendigt
#define LED_COUNT 16
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
//  36  1 Blå     16

const int switchPins[16] = {2, 4, 5, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35, 36}; // Eksempel på GPIO numre
bool lastSwitchStates[16] = {false};

void setup() {
  Serial.begin(115200);
  // Forbinder til Wi-Fi
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
  for (int i = 0; i < 16; i++) {
    pinMode(switchPins[i], INPUT_PULLUP); // Brug INPUT_PULLUP hvis dine kontakter er koblet til GND
    lastSwitchStates[i] = digitalRead(switchPins[i]);
  }
}

void loop() {
  timeClient.update();
  unsigned long currentEpochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&currentEpochTime);

  int hour = ptm->tm_hour;

  // Tænder alle LED'er røde kl. 7
  if (hour == 7) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0)); // Rød
    }
    strip.show();
  }

  // Tjekker hver kontakt og opdaterer LED'ernes farve
  for (int i = 0; i < 16; i++) {
    bool currentSwitchState = digitalRead(switchPins[i]);
    if (currentSwitchState != lastSwitchStates[i]) {
      lastSwitchStates[i] = currentSwitchState;
      if (currentSwitchState) {
        // Hvis kontakten er skiftet til ON, og det ikke er kl. 7, skift til grøn
        if (hour != 7) {
          strip.setPixelColor(i, strip.Color(0, 255, 0)); // Grøn
          strip.show();
        }
      }
    }
  }

  delay(1000); // Vent et sekund før næste loop iteration for at mindske belastningen
}

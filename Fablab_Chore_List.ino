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
//  2   2 Grå2    0
//  4   2 Lilla2  1 (Tænder både 1 og 3) tyder på forbindelse med naboledning
//  5   2 Blå     2 (Tænder både 2 og 4) tyder på forbindelse med naboledning
//  18  2 Grøn    3 
//  19  2 Gul     4
//  21  2 Orange  5
//  22  2 Rød     6
//  23  2 Brun    7
//  25  2 Sort    8
//  26  2 Hvid    9
//  27  2 Grå1    10
//
//  32  1 Sort2   11 (Virker ikke) pin eller kabel, fejl ikke fundet endnu
//  33  1 Hvid2   12
//  12  1 Grå     13
//  13  1 Lilla   14


const int switchPins[15] = {2, 4, 5, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 12, 13}; // Eksempel på GPIO numre
bool lastSwitchStates[15] = {false};

const int skiftHour   = 21;
const int skiftMinute = 35;
const int skiftSekund = 0;

int ledTest = 1; //Hvis 1 laver den test af LED'erne ved opstart, hvis 0 gør den ikke


//  .___________. __    __   _______         _______. _______ .___________. __    __  .______   
//  |           ||  |  |  | |   ____|       /       ||   ____||           ||  |  |  | |   _  \  
//  `---|  |----`|  |__|  | |  |__         |   (----`|  |__   `---|  |----`|  |  |  | |  |_)  | 
//      |  |     |   __   | |   __|         \   \    |   __|      |  |     |  |  |  | |   ___/  
//      |  |     |  |  |  | |  |____    .----)   |   |  |____     |  |     |  `--'  | |  |      
//      |__|     |__|  |__| |_______|   |_______/    |_______|    |__|      \______/  | _|      

void setup() {
//Start seriel forbindelse
Serial.begin(115200);

  // Initialiserer LED strip og strip test
  strip.begin();

if (ledTest == 0) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(random(5,10), random(1,5), random(10,15))); // Slukket
      Serial.print(i);
      Serial.println(" LED Tændt ");
      strip.show();
      delay(200);
    }
delay(2000);

    strip.begin();
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0)); // Slukket
      Serial.print(i);
      Serial.println(" LED SLUKKET");
      strip.show();
      delay(200);
    }
delay(2000);
}

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



 // Initialiserer kontakternes pins som input
for (int i = 0; i < 15; i++) {
    pinMode(switchPins[i], INPUT_PULLUP); // Brug INPUT_PULLUP kontakter er koblet til GND
    lastSwitchStates[i] = digitalRead(switchPins[i]);
}

}


//  .___________. __    __   _______     __        ______     ______   .______   
//  |           ||  |  |  | |   ____|   |  |      /  __  \   /  __  \  |   _  \  
//  `---|  |----`|  |__|  | |  |__      |  |     |  |  |  | |  |  |  | |  |_)  | 
//      |  |     |   __   | |   __|     |  |     |  |  |  | |  |  |  | |   ___/  
//      |  |     |  |  |  | |  |____    |  `----.|  `--'  | |  `--'  | |  |      
//      |__|     |__|  |__| |_______|   |_______| \______/   \______/  | _|      

void loop() {
  //Hent aktuel tid
  timeClient.update();
  unsigned long currentEpochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&currentEpochTime);

  //Aflæs hentet id og sæt hour til timetallet
  int hour = ptm->tm_hour;
  int minute = ptm->tm_min;
  int second = ptm->tm_sec;

  //Skift til LED'erne til rød vil kun ske når timerne er 7 om minutterne 00
  static bool updatedRed = false;
  if (hour == skiftHour && minute == skiftMinute && second == skiftSekund && !updatedRed) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(100, 0, 0)); // Rød
      Serial.print("LED ");
      Serial.print(i);
      Serial.println(" Rød");
    }
    strip.show();
    updatedRed = true;
  } else if (hour != skiftHour || minute != skiftMinute) {
    updatedRed = false;
  }



// Kontrollerer om kontakterne har skiftet posistion og ændre den tilhørende LED's farve
  for (int i = 0; i < LED_COUNT; i++) {
    bool currentSwitchState = digitalRead(switchPins[i]);
    //int ledIndex = i == 0 ? LED_COUNT - 1 : i - 1; // Mapper første kontakt til sidste LED, resten 1-til-1

    if (currentSwitchState != lastSwitchStates[i] && currentSwitchState == LOW) {
      lastSwitchStates[i] = currentSwitchState;
      if (!(hour == skiftHour && minute == skiftMinute && second == skiftSekund)) {
        strip.setPixelColor(LED_COUNT-i-1, strip.Color(0, 100, 0)); // Grøn
        strip.show();
        Serial.print(hour);
        Serial.print(":");
        Serial.print(minute);
        Serial.print(" - LED ");
        Serial.print(i);
        Serial.println(" Grøn");
      }
    } else {
      lastSwitchStates[i] = currentSwitchState;
    }
  }

  delay(100);

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

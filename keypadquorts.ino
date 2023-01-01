#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Keypad.h>
#include "time.h"
#include "stdio.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// Set these to run example.
#define FIREBASE_HOST "quorts-6b410.firebaseio.com"
#define FIREBASE_AUTH "LCSKtDPzDtkTCcKc48ZFU3QnZS7tghiB108P5o11"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -28800;   //Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 3600;  //Replace with your daylight offset (seconds)

const byte ROWS = 4; 
const byte COLS = 3; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {3, D3, D4, D5}; 
byte colPins[COLS] = {D6, D7, D8}; 
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
LiquidCrystal_I2C lcd(0x3F, 16, 2);

#define WIFI_SSID "SRVUSD-GUEST"

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  // connect to wifi.
  WiFi.begin(WIFI_SSID);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //printLocalTime();
  //printLocalTime();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;

void loop() {
  // set value
  char customKey = customKeypad.getKey();
  if(customKey){
    String cks = ""; cks += customKey;
    delay(1500);
    lcd.print("UPDATED: " + cks);
    delay(1000);
    lcd.clear();
    delay(100);
  }
  if(customKey){
    // set string value
    String cks = ""; cks += customKey;
    if (cks == "*" or cks == "#" or cks == "9") {
      lcd.setCursor(0,0);
      lcd.print("ERROR: Enter Num");
      lcd.setCursor(6, 1);
      lcd.print("#0-8");
      delay(1000);
      lcd.clear();
      delay(100);
      return;
    }
    Serial.println(cks);
    Firebase.setString("first/Dougherty Valley/courtsOpen", cks);
    // handle error
    if (Firebase.failed()) {
        Serial.print("setting /message failed:");
        Serial.println(Firebase.error());  
        return;
    }
    delay(500);
    String hold = printLocalTime();
    while(hold[0] == '1') hold = printLocalTime();
    Firebase.setString("first/Dougherty Valley/lastTime", hold);
    
    // handle error
    if (Firebase.failed()) {
        Serial.print("setting /message failed:");
        Serial.println(Firebase.error());  
        return;
    }
    lcd.print("UPDATED: " + cks);
    delay(1000);
    lcd.clear();
    delay(100);
  }
}
String printLocalTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [80];
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (buffer,80,"%F %I:%M:%S %p",timeinfo);
  puts (buffer);
  String str(buffer);
  str = buffer;
  return str;
  delay(1000);
}

/*Refer to
https://randomnerdtutorials.com/esp8266-nodemcu-http-get-open-weather-map-thingspeak-arduino/
*/
#include "DHT.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

//#include <MQ2.h>

/* LCD CONSTANTS*/

LiquidCrystal_I2C lcd(0x3F, 20, 4); // SDA : D2 ; SCL : D1

/* DHT11 constants declarations */ 

#define DHTPIN 0// D3 on NodeMCU is GPIO 0

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define lcd_light 16 //backlight pin D0

/*MODE BUTTON*/

uint8_t mode = D7;
int count=0;

/* NTP TIME CONSTANTS*/

const char *ssid     = "The Den";
const char *password = "this_is_a_scam";
const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"asia.pool.ntp.org",utcOffsetInSeconds);
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
String ntpDate ="";


/*MQ2 sensor constants*/
/*int pin = A0;
int lpg, co, smoke;
MQ2 mq2(pin);
*/
/* Open Weather Constants */
String openWeatherMapApiKey = "fca476e216674e4b517d418e204ba718"; //API key
String city = "Kolkata";                                          //City
String countryCode = "IN";                                        //Country code
unsigned long lastTime = 0;                                       //dont change
unsigned long timerDelay = 10000;                                 //dont change
String jsonBuffer;

String weatherType = "";


void setup() {
  
  Serial.begin(115200);
  
  //mq2.begin();
  
  pinMode(mode,INPUT);
  
  lcd.begin();
  dht.begin();
  lcd.backlight();
  pinMode(lcd_light,OUTPUT);
  digitalWrite(lcd_light,HIGH);
  lcd.setCursor(0,0);
  
  lcd.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  lcd.setCursor(0,1);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
    
    lcd.print(".");
  }

  timeClient.begin();
  //+lcd.clear();

}
void wifi_cred()
{
  Serial.println(ssid);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ssid : ");
  lcd.print(ssid);
  lcd.setCursor(0,1);
  //lcd.print("lIP:");
  lcd.print(WiFi.localIP());
  Serial.println(WiFi.localIP());
  delay(3000);
}

void forecast()
{
    // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&lang=PT&mode=json&units=metric&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);

      String weatherType = myObject["weather"][0]["main"];

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Temp: ");
      lcd.print(myObject["main"]["temp"]);
      lcd.print((char)223);
      lcd.print("C");
      lcd.setCursor(0,1);
      lcd.print("Hum: ");
      lcd.print(myObject["main"]["humidity"]);
      lcd.print("%");
      lcd.setCursor(0,2);
      lcd.print("Pres: ");
      lcd.print(myObject["main"]["pressure"]);
      lcd.print("hPa");
      lcd.setCursor(0,3);
      lcd.print("W-Speed:");
      lcd.print(myObject["wind"]["speed"]);
      lcd.print("m/s");

      delay(3500);

      lcd.clear();
      
      lcd.setCursor(0,0);
      //lcd.print("Forecast:");
      //lcd.setCursor(0,1);
      lcd.print(weatherType);
      lcd.setCursor(0,1);
      lcd.print("Min_Temp: ");
      lcd.print(myObject["main"]["temp_min"]);
      lcd.print((char)223);
      lcd.print("C");
      lcd.setCursor(0,2);
      lcd.print("Max_Temp: ");
      lcd.print(myObject["main"]["temp_max"]);
      lcd.print((char)223);
      lcd.print("C");
      lcd.setCursor(0,3);
      lcd.print("Feels like: ");
      lcd.print(myObject["main"]["feels_like"]);
      lcd.print((char)223);
      lcd.print("C");
      //lcd.print();

      //delay(500);

      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);         //degree celcius
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);     //hPa (Hecto Pascals)
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);     //percentage %
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
     
      Serial.println(weatherType);

      // weather_id = myObject["weather"][0]["id"];
      /*
      if (weather_id > 800 && weather_id <= 804)
      { weatherType = "Cloudy";}
      else if (weather_id >= 300 && weather_id < 600)
      { weatherType = "Rain";}
      else if (weather_id >= 200 && weather_id < 300)
      { weatherType = "Thunder Storm";}
      else if (weather_id >= 700 && weather_id < 800)
      { weatherType = "Haze";}
      else if (weather_id == 800)
      { weatherType = "Clear";}
*/    
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
void time()
{
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  lcd.setCursor(0,0);
  lcd.print("Connected to ");
  lcd.print(ssid);
  lcd.setCursor(0,1);
  //lcd.print("TIME :");
  lcd.print(timeClient.getFormattedTime());
  lcd.setCursor(0,2);
  ntpDate = timeClient.getFormattedDate();
  int ind = ntpDate.indexOf("T");
  lcd.print(ntpDate.substring(0,ind));
  Serial.println(ntpDate.substring(0,ind));
  //lcd.print("DATE : ");
  lcd.print(" ");
  lcd.print(daysOfTheWeek[timeClient.getDay()]);
  
  Serial.println(daysOfTheWeek[timeClient.getDay()]);
  if(int(timeClient.getHours())<7)
    digitalWrite(lcd_light,LOW);
  else
    digitalWrite(lcd_light,HIGH);
  delay(1000);
}
void temp()
{
  lcd.clear();
  float h = dht.readHumidity();
   float t = dht.readTemperature();
   float f = dht.readTemperature(true);
   if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    lcd.setCursor(0,0);
    lcd.print("Sensor Error");
    delay(500);
    return;
   }
   Serial.println(F("Humidity: "));
   Serial.print(h);
   lcd.setCursor(0,0);
   lcd.print("HUM: ");
   lcd.print(h);
   lcd.print("%");
   Serial.print(F("%  Temperature: "));
   Serial.print(t);
   Serial.print(F("°C "));
   lcd.setCursor(0,1);
   lcd.print("TEM: ");
   lcd.print(t);
   lcd.print((char)223);  
   lcd.print("C");
   delay(500);
}


/*
void air_quality()
{
  float* values= mq2.read(true);
  lpg = mq2.readLPG();
  co = mq2.readCO();
  smoke = mq2.readSmoke();
  
  lcd.setCursor(0,0);
  lcd.print("CO: ");
  lcd.print(co);
  
  lcd.setCursor(0,1);
  lcd.print("Smoke: ");
  lcd.print(smoke);
  
  delay(3000);
}
*/
void loop() {
   if(digitalRead(mode)==HIGH)
   {
      count+=1;
   }
   

   if (count==0)
   {
     lcd.clear();
     time();
   }
   
   else if(count==1)
   {
    lcd.clear();
    temp();
   }


   else if(count==2)
   {
     //lcd.clear();
     forecast();
   }
   

   else if(count==3)
   {
    lcd.clear();
    wifi_cred();
    count=0;
   }

}

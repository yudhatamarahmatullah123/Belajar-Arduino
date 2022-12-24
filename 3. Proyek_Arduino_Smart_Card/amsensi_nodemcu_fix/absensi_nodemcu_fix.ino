#include <Adafruit_LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>        // version 6.13
#include <SPI.h>
#include <RFID.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#define alamatRTC 0x68
#define alamatEEPROMCekToken 0
#define tokenRTC 0xAA //<== rubah token jika ingin nilai baru

WiFiClient wifiClient;
RTC_DS3231 rtc;

char t[32];

LiquidCrystal_I2C lcd(0x27, 20, 4);       // sesuaikan alamat i2c (0 x27) dengan alamat i2c kalian

#define SS_PIN D4
#define RST_PIN D3
#define Buzzer D8
#define lampu D0

//const char* wifiName = "SMART CARD";
//const char* wifiPass = "smc2021oke";

//const char* wifiName = "what";
//const char* wifiPass = "smc2021okee";

const char* wifiName = "wifi_premium";
const char* wifiPass = "tanyasayyidul";

//const char* wifiName = "LAB_MULTIMEDIA";
//const char* wifiPass = "markazlughoh2020";

//const char* wifiName = "Kantor Bapak";
//const char* wifiPass = "dkpunida";

const String iddev = "98";
const long utcOffsetInSeconds = 25200;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "id.pool.ntp.org", utcOffsetInSeconds);

//String hostMode = "http://absensi.xyz/api/getmodejson?key=mrRkmgf82kjrJNkVYjsfnxz&iddev=" + iddev;
//String hostSCAN = "http://172.20.20.7:3015/rfid/api/input/db";
//String hostADD = "http://172.20.20.7:3015/rfid/api/input/db";

//String hostMode = "http://absensi.xyz/api/getmodejson?key=mrRkmgf82kjrJNkVYjsfnxz&iddev=" + iddev;
//String hostSCAN = "http://172.20.20.7:3015/smcard/perizinan/keluar";
//String hostADD = "http://172.20.20.7:3015/smcard/perizinan/keluar";

//String hostMode = "http://absensi.xyz/api/getmodejson?key=mrRkmgf82kjrJNkVYjsfnxz&iddev=" + iddev;
String hostSCAN = "http://api.unida.gontor.ac.id:1926/event/checkin";
String hostADD = "http://api.unida.gontor.ac.id:1926/event/checkin";

//String hostMode = "http://absensi.xyz/api/getmodejson?key=mrRkmgf82kjrJNkVYjsfnxz&iddev=" + iddev;
//String hostSCAN = "http://172.20.20.7:3015/smcard/perizinan/masuk";
//String hostADD = "http://172.20.20.7:3015/smcard/perizinan/masuk";

//String hostMode = "http://absensi.xyz/api/getmodejson?key=mrRkmgf82kjrJNkVYjsfnxz&iddev=" + iddev;  //cek website
//String hostSCAN = "http://172.20.20.7:3015/smcard/perizinan";
//String hostADD = "http://172.20.20.7:3015/smcard/perizinan";


String ModeAlat = "SCAN"; 

RFID rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  timeClient.begin();
  
  //Wire.begin(5, 4);   //Setting wire (5 untuk SDA dan 4 untuk SCL)
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));  //Setting Time
  
  // Kalian dapat menambahkan bagian dibawah ini untuk set manual jam
  //rtc.adjust(DateTime(2021, 8, 2, 19, 16, 20));
  SPI.begin();
  rfid.init();
  delay(10);

  pinMode(Buzzer, OUTPUT);
  pinMode(lampu, OUTPUT);
  
  Wire.begin(D2,D1);
  //lcd.begin();
  //lcd.home ();
  //lcd.print("RFID Reader Absensi");  
  delay (1000);
  Serial.println();
  
  Serial.print("Connecting to ");
  Serial.println(wifiName);

  lcd.setCursor(0,1);
  lcd.print("Connecting to");
  lcd.setCursor(0,2);
  lcd.print(wifiName);  

  WiFi.begin(wifiName, wifiPass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP

//  ModeDevice();
}

void ModeDevice(){
  HTTPClient http;

  Serial.print("Request Link:");
  Serial.println(hostSCAN);
  
  http.begin(wifiClient,hostSCAN);
  
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload from server

  
  Serial.print("Response Code:"); //200 is OK
  Serial.println(httpCode);       //Print HTTP return code

  Serial.print("Returned data from Server:");
  Serial.println(payload);    //Print request response payload
  
 
  http.end();

  delay(100);
}

void loop() {

  timeClient.update();

//  delay(100);

  if (ModeAlat == "SCAN"){
    Serial.println("SCAN RFID CARD");
    digitalWrite(lampu, HIGH);
    if (rfid.isCard()) {
      if (rfid.readCardSerial()) {
          //Serial.println(rfid.serNum.length());
          digitalWrite(lampu,LOW);


          Serial.println("");
          Serial.println("Card found");
          DateTime now = rtc.now();       //Menampilkan RTC pada variable now

          String kodeVenue = "TUSAINTEK";
          //String kodeVenue = "AUSTMAN";
          //String kodeVenue = "ACIOS";
          //String kodeVenue = "AGETER4";
          //String kodeVenue = "AGETERLT1";          
          //String kodeVenue = "AGETERLT3";
          
          String RFID     = String(rfid.serNum[0],HEX) +"-"+ String(rfid.serNum[1],HEX) +"-"+ String(rfid.serNum[2],HEX) +"-"+ String(rfid.serNum[3],HEX) +"-"+ String(rfid.serNum[4],HEX);
          String tanggal2 = String(now.year())+"-"+String (now.month())+"-"+String  (now.day(), DEC)+"%20"+String (timeClient.getHours())+"%3A"+String (timeClient.getMinutes())+"&3A"+String (timeClient.getSeconds());
          String msg      = kodeVenue+"%23"+RFID+"%23"+tanggal2;

          
          String host  = hostADD;
          host        += "?msg=";
          host        += msg;

          HTTPClient http;
  
          http.begin(wifiClient,host);
          http.addHeader("x-access-token","eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjbGllbnRJZCI6IjE5MjYyMDE0IiwiY2xpZW50U2VjcmV0IjoiYmlzbWlsbGFoIiwiaWF0IjoxNTc5MzE4NzA3fQ.5Z43hVcfu8d-0Sqn46r9eC6XEq97cMc-CrZBJFEs5tA");
          int httpCode = http.GET();            //Send the GET request
          String payload = http.getString();    //Get the response payload from server
        
          Serial.print("Response Code:"); //200 is OK
          Serial.println(httpCode);       //Print HTTP return code
        
          Serial.print("Returned data from Server:");
          Serial.println(payload);    //Print request response payload
                    Serial.print(RFID); //200 is OK

/*
          if(payload == "200")        //SET BUZZER ADA ATAU TIDAKNYA EVENT
            {
               digitalWrite(Buzzer,HIGH);
                delay(1000);
                digitalWrite(Buzzer,LOW);
            }else if(payload == "505"){
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);
                delay(50);
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);
                delay(50);
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW); 
            }else if(payload == "500"){
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);
                delay(50);
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);
                delay(50);
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW); 
                delay(50);
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);
                delay(50);
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);  
              }else{
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);
                delay(50);
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);
                delay(50);
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);
                delay(50);
                digitalWrite(Buzzer,HIGH);
                delay(100);
                digitalWrite(Buzzer,LOW);  
      }*/

               int modelBuzzer = payload.toInt();
                
               
               for(int x = 1; x <= modelBuzzer; x++){
                  digitalWrite(Buzzer, HIGH);
                  delay(100);
                  digitalWrite(Buzzer, LOW);
                  delay(100);
               }

      
    }
    
    
    
    
    
    
    
    
    else{
      Serial.println("WAITING RFID CARD");
      
      lcd.setCursor(0,2);
      lcd.print("Menunggu Kartu RFID ");
      
    }
    
    rfid.halt();
    delay(1000);
  }
  

  else{
    Serial.println("Tidak Mendapatkan MODE ALAT dari server");
    Serial.println("Cek IP Server dan URL");
    Serial.println("Restart NodeMCU");
    lcd.setCursor(0,1);
    lcd.print("MODE ALAT ERROR");
    lcd.setCursor(0,2); 
    Serial.println("Cek IP Server dan URL");
    lcd.setCursor(0,3);
    Serial.println("Restart NodeMCU");
    delay(1000);         
  }
}
  }

  

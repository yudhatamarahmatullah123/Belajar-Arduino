#include <SPI.h>
#include <Ethernet.h>
#include <RFID.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

    // CONFIG ethernet
byte mac[]    = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[]     = {192, 168, 208, 99};                                 // static arduino IP
byte server[] = {192, 168, 208, 129};                                // static server IP
EthernetClient client;                                             

    // CONFIG RFID
#define SS_PIN 9
#define RST_PIN 8
RFID rfid(SS_PIN, RST_PIN);             

    // CONFIG LCD
//LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
LiquidCrystal_I2C lcd(0x27, 16, 2);

    // CONFIG LED
int ledMerah = 4;     // Lampu LED warna MERAH
int ledHijau = 5;     // Lampu LED warna HIJAU

     // CONFIG BUZZER
const int buzzer = 3;

    // CONFIG WEB SERVER
int    HTTP_PORT   = 1926;
String HTTP_METHOD = "GET";
char   HOST_NAME[] = "api.unida.gontor.ac.id"; // hostname of web server:
String PATH_NAME   = "/event/checkin";
String TOKEN       = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjbGllbnRJZCI6IjE5MjYyMDE0IiwiY2xpZW50U2VjcmV0IjoiYmlzbWlsbGFoIiwiaWF0IjoxNTc5MzE4NzA3fQ.5Z43hVcfu8d-0Sqn46r9eC6XEq97cMc-CrZBJFEs5tA";

  void setup(){    
     Serial.begin(9600);
     SPI.begin(); 
     lcd.init();      //initialize the lcd
     lcd.backlight(); //open the backlight 
     rfid.init();
     lcd.begin(16, 2);
     
     lcd.setCursor(0, 0);
     lcd.print(F("Device setup.."));
     
         // Disable rfid
     pinMode(9, OUTPUT);
     digitalWrite(9, HIGH);
     
         // Disable sd card
     pinMode(4, OUTPUT);
     digitalWrite(4, HIGH);
      
         // Enable ethernet
     pinMode(10, OUTPUT);
     digitalWrite(10, LOW); 
     
         // Setup buzzer (pin 2)
     pinMode(buzzer, OUTPUT);

         // Setup Lampu led
     pinMode(ledMerah, OUTPUT);
     pinMode(ledHijau, OUTPUT);
     
        // Acquiring ip without dhcp
     Ethernet.begin(mac, ip);
     delay(1000);
     lcd.setCursor(14, 0);
     lcd.print(F("OK"));
     delay(500);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print(F("    UNIDA-01     "));
     lcd.setCursor(0, 1);
     lcd.print(F("Waiting server.."));
     
        // Try check connect to web server
     if(client.connect(HOST_NAME, HTTP_PORT)) {
        Serial.println("Connected to server");
        lcd.setCursor(0, 1);
        lcd.print(F("  Connected...  "));
     } else {
        Serial.println("connection failed");
        lcd.setCursor(0, 1);
        lcd.print(F("Koneksi Gagal!!!"));
     }
  }

  void loop(){      
     if (rfid.isCard()) {
        if (rfid.readCardSerial()) { 
          Serial.println("Trying to connect...");
            if(client.connect(HOST_NAME, HTTP_PORT)) {
               String id = String(rfid.serNum[0],HEX) +"-"+ String(rfid.serNum[1],HEX) +"-"+ String(rfid.serNum[2],HEX) +"-"+ String(rfid.serNum[3],HEX) +"-"+ String(rfid.serNum[4],HEX);
               lcd.setCursor(0, 1);
               lcd.print("   Card Found    ");
               lcd.setCursor(0, 1);
               lcd.print("ID " + id);
               delay(250);
               lcd.clear();
               lcd.setCursor(0, 1);
               lcd.print(F("  Process...  "));
               delay(250);
               Serial.println("Kode rfid = " + id);
               lcd.setCursor(0, 0);
               lcd.print(F("    UNIDA-01    "));
               
                    // Send HTTP header
               String kodeVenue = "SmartCard";
               String msg = kodeVenue+"%23"+id;  
               String host = "?msg=";
                      host += msg;
               client.println(HTTP_METHOD + " " + PATH_NAME + host + " HTTP/1.1");
               client.println("Host: " + String(HOST_NAME));
               client.println("x-access-token: " + TOKEN);
               client.println("Connection: close");
               client.println(); 
                    // end HTTP header 

               digitalWrite(ledHijau, HIGH);

                    // Progcess output form server
               Serial.println("Waiting data.....");              
               
               String pt = "";               
               
               while(client.connected()) {
                  if(client.available()) {
                      // read an incoming byte from the server and print it to serial monitor:
                      char c = client.read();
                      pt = c; 
                  }
               }

               Serial.println(pt);  
               client.stop();

               int modelBuzzer = pt.toInt();
               digitalWrite(ledMerah, LOW);

               for(int x = 1; x <= modelBuzzer; x++){
                  digitalWrite(buzzer, HIGH);
                  digitalWrite(ledHijau, HIGH);
                  delay(100);
                  digitalWrite(buzzer, LOW);
                  digitalWrite(ledHijau, LOW);
                  delay(100);
               }

               lcd.setCursor(0, 1);
               lcd.print(F("Waiting server.."));
               delay(1000);
               lcd.setCursor(0, 1);
               lcd.print(F("  Connected...  "));
               digitalWrite(ledMerah, LOW);
            }
                       
           else {
             Serial.println("connection failed");
             lcd.setCursor(0, 1);
             lcd.print(F("Koneksi Gagal!!!"));
           }
            
        } 
     } 
             // Custom led desaign
        digitalWrite(ledMerah, HIGH);
        delay(250);
        digitalWrite(ledMerah, LOW);
        delay(250);
        digitalWrite(ledMerah, HIGH);
  }

  // 1 = Berhasil
  // 2 = Error
  // 3 = Tidak ada event bagi mahasiswa

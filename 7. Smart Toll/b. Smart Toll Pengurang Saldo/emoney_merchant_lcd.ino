/*
 * Peringatan : Data akan di simpan pada RFID Card pada sector #1 (blocks #4).
 *
 * tytomulyono.com
 * Typical pin layout used:
 * --------------------------------------
 *             MFRC522      Arduino      
 *             Reader/PCD   Uno/101      
 * Signal      Pin          Pin           
 * --------------------------------------
 * RST/Reset   RST          9             
 * SPI SS      SDA(SS)      10            
 * SPI MOSI    MOSI         11 / ICSP-4   
 * SPI MISO    MISO         12 / ICSP-1   
 * SPI SCK     SCK          13 / ICSP-3   
 * tytomulyono.com
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 20, 4); 

#define RST_PIN         9
#define SS_PIN          10

MFRC522 mfrc522(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

long tagihan = 10000;            // isikan tagihan merchant kelipatan 1000, max 255rb

bool notif = true;

long saldo;
int digit;

long OLDsaldo;
int OLDdigit;

void setup() {
    Serial.begin(9600);
    SPI.begin();        
    mfrc522.PCD_Init(); 

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println("EMONEY Tanpa Server - Merchant");
    Serial.println();
    Serial.println("Peringatan : Data akan di simpan pada RFID Card pada sector #1 (blocks #4)");
    Serial.println();

    lcd.begin();
    lcd.home ();
    lcd.print("EMONEY - Merchant");  
    
    if (tagihan > 255000){
      while(1){
        Serial.println("ERROR - Tagihan merchant lebih dari 255rb");
      }
    }

    Serial.print("Tagihan Merchant : ");
    Serial.println(tagihan);

    lcd.setCursor(0,1);
    lcd.print("Harga ");
    lcd.print(tagihan);
    
    digit = tagihan/1000;
    //Serial.println(digit);
}

void loop() {
  if (notif){
    notif = false;
    Serial.println();
    Serial.println("________________________________________________________________________________");
    Serial.println("Silahkan tap kartu");
    Serial.print("Setiap TAP, saldo berkurang sebesar ");
    Serial.println(tagihan);
  }
  if ( ! mfrc522.PICC_IsNewCardPresent()){
      return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()){
      return;
  }
  
  Serial.println();
  Serial.print("Card UID:");
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  
  Serial.println();
  Serial.print("Tipe Kartu : ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Cek kesesuaian kartu
  if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
      &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
      &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      Serial.println(F("Kode ini hanya dapat digunakan pada MIFARE Classic cards 1KB - 13.56MHz."));
      notif = true;
      delay(2000);
      resetReader();
      return;
  }

  // that is: sector #1, covering block #4 up to and including block #7
  byte sector         = 1;
  byte blockAddr      = 4;
  
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  //Serial.println("Current data in sector:");
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

  // Baca Saldo yang ada dari RFID Card
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
      Serial.println("Gagal Baca Kartu RFID");
      //Serial.println(mfrc522.GetStatusCodeName(status));
      resetReader();
      return;
  }
  OLDdigit = buffer[0];
  OLDsaldo = OLDdigit;
  OLDsaldo *= 1000;
  Serial.print("Saldo Kartu Sebelumnya : ");
  Serial.println(OLDsaldo);
  Serial.println();
  lcd.setCursor(0,2);
  lcd.print("Saldo Kartu");
  lcd.setCursor(0,3);
  lcd.print(OLDsaldo);
  delay(2000);
  lcd.setCursor(0,2);
  lcd.print("                   ");
  lcd.setCursor(0,3);
  lcd.print("                   ");

  // Kurangi Saldo sebesar tagihan merchant
  if (OLDdigit < digit){
    Serial.println("Saldo belum di kurangi, saldo tidak cukup, silahkan isi saldo terlebih dahulu");
    Serial.println("GAGAL bayar merchant");
    lcd.setCursor(0,2);
    lcd.print("GAGAL Bayar");
    lcd.setCursor(0,3);
    lcd.print("Saldo Kurang");
    delay(2000);
    lcd.setCursor(0,2);
    lcd.print("                   ");
    lcd.setCursor(0,3);
    lcd.print("                   ");
  
    resetReader();
    return;
  }

  OLDdigit -= digit;
  
  byte dataBlock[]    = {
      //0,      1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15
      OLDdigit, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
      Serial.println("GAGAL Write Saldo pada Kartu RFID");
      //Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
      Serial.println("Gagal Baca Kartu RFID");
      //Serial.println(mfrc522.GetStatusCodeName(status));
  }

  Serial.println();

  Serial.println("Mengurangi Saldo...");
  if (buffer[0] == dataBlock[0]){
    saldo = buffer[0];
    saldo *= 1000;
    //Serial.print("data digit ke 0 : ");
    //Serial.println(buffer[0]);
    Serial.print("======================>>>>>> Saldo kartu sekarang : ");
    Serial.println(saldo);
    Serial.println("_________ Berhasil bayar Merchant ___________");
    lcd.setCursor(0,2);
    lcd.print("BERHASIL Bayar");
    lcd.setCursor(0,3);
    lcd.print("Sisa Saldo ");
    lcd.print(saldo);
    delay(2000);
    lcd.setCursor(0,2);
    lcd.print("                   ");
    lcd.setCursor(0,3);
    lcd.print("                   ");
  }else{
    Serial.println("------------ GAGAL bayar Merchant --------------");
  }

  Serial.println();

  // Dump the sector data
  //Serial.println("Current data in sector:");
  //mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

  resetReader();
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void resetReader(){
  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();

  notif = true;
}

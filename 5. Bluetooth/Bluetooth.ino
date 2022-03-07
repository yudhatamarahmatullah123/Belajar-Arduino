#include <SoftwareSerial.h> 
SoftwareSerial module_bluetooth(0, 1); // pin RX | TX
 
char data = 0;             
void setup() 
{
  Serial.begin(9600);         
  pinMode(8, OUTPUT);  //inisialisasi LED menjadi output
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}
void loop()
{
  if(Serial.available() > 0)  
  {
    data = Serial.read();Serial.print(data);    //Pembacaan dan ditampilkan data yang masuk   
    Serial.print("\n"); 
    //Data yang masuk      
    if(data == '0'){
    digitalWrite(8, HIGH); //diberikan waktu tunda 1 detik
    }
    else if(data == '1'){      
    digitalWrite(8, LOW); 
    } 
    else if(data == '2'){      
    digitalWrite(9, HIGH); 
    } 
    else if(data == '3'){      
    digitalWrite(9, LOW); 
    }
    else if(data == '4'){      
    digitalWrite(10, HIGH); 
    }
    else if(data == '5'){      
    digitalWrite(10, LOW); 
    }
    else if(data == '6'){      
    digitalWrite(11, HIGH); 
    }
    else if(data == '7'){      
    digitalWrite(11, LOW); 
    }
     else if(data == '8'){      
    digitalWrite(8, LOW); 
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }      
  else if(data == '9'){      
    digitalWrite(8, HIGH); 
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);
  }                            
 
}}

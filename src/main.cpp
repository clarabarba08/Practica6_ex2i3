// LECTOR TARGETA - Control accessos

//Libraries
#include <Arduino.h>
#include <FS.h>
#include <SPI.h> //https://www.arduino.cc/en/reference/SPI
// #include <MFRC522.h> //https://github.com/miguelbalboa/rfid
#include <MFRC522Extended.h>
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" 
// Optionally include custom images
#include "images.h"

//Constants
#define RST_PIN 17 //Pin para el reset del RC522
#define SS_PIN 5 //Pin  para el SS (SDA) del RC522
// Clk al pin 18
// MOSI al pin 23
// MISO al pin 19

#define SCL_PIN 22
#define SDA_PIN 16

#define LedVerde 27
#define LedRojo 12

#define USERS 1
byte validKey1[USERS][5] = {{0x53,0x3E,0x18,0x0B}}; //,{0xD0,0x1C,0x85,0x21}};
                            
char* names[USERS]={"Targeta blanca Clara"}; // ,"Clauer blau"}; 

//Parameters
const int ipaddress[4] = {103, 97, 67, 25};

//Variables
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el RC522
// MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);  //Creamos el objeto para el RC522

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA_PIN, SCL_PIN); 



void printDec(byte *buffer, byte bufferSize);
void readRFID(void );
void printHex(byte *buffer, byte bufferSize) ;


#define DEMO_DURATION 3000
typedef void (*Demo)(void);
void init_wifi();
int demoMode = 0;
int counter = 1;


void setup() {

  pinMode(LedVerde,OUTPUT);   
  pinMode(LedRojo,OUTPUT);  
  //Init Serial USB
Serial.begin(115200);
Serial.println(F("Inicialitzar el sisitema"));
  //init rfid D8,D5,D6,D7
  SPI.begin();  //Iniciamos el Bus SPI
 mfrc522.PCD_Init(); // Iniciamos  el MFRC522

  Serial.print(F("Lector:"));
 mfrc522.PCD_DumpVersionToSerial();

    // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(128/2, 10, "CONTROL D'ACCÉS: Passa una targeta");
  display.drawRect(0, 0, 128, 64);
  display.display();

}

// funció per llegir targetes i mostrar el seu UID: 
void readRFID(void ) { /* function readRFID */
  ////Read RFID card
// Revisamos si hay nuevas tarjetas  presentes
	if ( mfrc522.PICC_IsNewCardPresent()) 
        {  
  		//Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  Serial.print("Card UID:");
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);   
                  } 
                  Serial.println();
                  // Terminamos la lectura de la tarjeta  actual
                  mfrc522.PICC_HaltA();         
            }      
	}	

}


/**
   Helper routine to dump a byte array as hex values to Serial. (no utilitzat)
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
   Helper routine to dump a byte array as dec values to Serial. (no utilitzat)
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

 //comparar el ID de la targeta introduida amb els ID de les targetes autoritzades
bool checkData(byte* arrayA, byte* arrayB, int length)
{
  for (int index = 0; index < length; index++)
  {
    if (arrayA[index] != arrayB[index]) return false;
  }
  return true;
}

bool checkAuthorization(byte *data) //Verify Access
{
    for(int i = 0; i<USERS; i++)
    {
      if(checkData(data,validKey1[i],5))
      {
        Serial.print("Usuari:");
        Serial.println(names[i]);

        return true;
      }
    }
   return false;
}


void loop() {
 // readRFID();
 // clear the display
  display.clear();
  display.drawRect(0, 0, 128, 64);
  display.display();

// PER ENCENDRE LEDS
 if (mfrc522.PICC_IsNewCardPresent()) /* Card Exist? */
  {
    mfrc522.PICC_ReadCardSerial();   /* Get serial number Card*/
    Serial.println("CONTROL DE ACCESO");
    //imprimir codigo de tarjeta
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
    //Serial.println(RC522.serNum[i],DEC);
      Serial.println(mfrc522.uid.uidByte[i],HEX); //Card in Hexadecimal
    }
    //----------------------------------------------
    // si el ID de la targeta està a la llista
     if(checkAuthorization(mfrc522.uid.uidByte))
    { 
      Serial.println("Accés: Autoritzat");
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(128/2, 20, "Accés: Autoritzat");
  display.display(); // per mostrar els canvis  
      digitalWrite(LedRojo, LOW);
      digitalWrite(LedVerde, HIGH);
      delay(2000);
      digitalWrite(LedRojo, LOW);
      digitalWrite(LedVerde, LOW);

    }
    else // si el ID de la targeta no està a la llista
    {
      Serial.println("Accés: Denegat");
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(128/2, 20, "Accés: Denegat");
  display.display();
      digitalWrite(LedRojo, HIGH);
      digitalWrite(LedVerde, LOW);
      delay(2000);
      digitalWrite(LedRojo, LOW);
      digitalWrite(LedVerde, LOW);     
    }
    Serial.println();
    Serial.println();
  }

  delay(20);
  
  // draw the current demo method
  //demos[demoMode]();

  /* display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 54, String(millis()/3600000)+String(":")\
          +String((millis()/60000)%60)+String(":")\
          +String((millis()/1000)%(60))); */
  delay(100);
}





////// pel display´
void drawFontFaceDemo() {
  // Font Demo1
  // create more fonts at http://oleddisplay.squix.ch/
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Hello world");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Hello world");
}

void drawTextFlowDemo() {
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0, 0, 128,
                             "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore." );
}

void drawTextAlignmentDemo() {
  // Text alignment demo
  display.setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 10, "Left aligned (0,10)");

  // The coordinates define the center of the text
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 22, "Center aligned (64,22)");

  // The coordinates define the right end of the text
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 33, "Right aligned (128,33)");
}

void drawRectDemo() {
  // Draw a pixel at given position
  for (int i = 0; i < 10; i++) {
    display.setPixel(i, i);
    display.setPixel(10 - i, i);
  }
  display.drawRect(12, 12, 20, 20);

  // Fill the rectangle
  display.fillRect(14, 14, 17, 17);

  // Draw a line horizontally
  display.drawHorizontalLine(0, 40, 20);

  // Draw a line horizontally
  display.drawVerticalLine(40, 0, 20);
}

void drawCircleDemo() {
  for (int i = 1; i < 8; i++) {
    display.setColor(WHITE);
    display.drawCircle(32, 32, i * 3);
    if (i % 2 == 0) {
      display.setColor(BLACK);
    }
    display.fillCircle(96, 32, 32 - i * 3);
  }
}

void drawProgressBarDemo() {
  int progress = (counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}

void drawImageDemo() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

Demo demos[] = {drawFontFaceDemo, drawTextFlowDemo, drawTextAlignmentDemo, drawRectDemo, drawCircleDemo, drawProgressBarDemo, drawImageDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;
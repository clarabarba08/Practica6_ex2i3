# PRÀCTICA 6
## EXERCICI 2 i 3: Lector targetes RFID
He ajuntat els dos exercicis en un codi ja que l'exercici 3 conté el codi proporcionat en l'exercici 2. 
He creat un control d'accessos. Es passa una targeta pel lector i si pot passar s'ilumina un led verd i per un display podem veure *Accés autoritzat* i si no pot passar s'encén un led vermell i pel dispaly veiem *Accés denegat*. 

Per decidir qui passa i qui no, s'ha d'establir prèviament quines targetes tenen accés i quines no. Això es fa a través de l'ID de cada targeta. Com que tinc dues targetes he guardat només l'ID d'una i de l'altra no. Primer hem de conèixer l'ID de la targeta que volem autoritzar-li l'accés. Per això cal el següent codi: 
```cpp
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
// Enviamos serialemente su UID
                  Serial.print("Card UID:");
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);   
                  } 
```
Ara quan passem una targeta pel lector podem veure el seu ID per pantalla. En el cas de la meva targeta blanca el ID és: 0x53,0x3E,0x18,0x0B. Com que només volem guardar aquesta targeta el codi serà el següent: 
```cpp 
#define USERS 1
byte validKey1[USERS][5] = {{0x53,0x3E,0x18,0x0B}};  
char* names[USERS]={"Targeta blanca Clara"}; 
```
En el vídeo del funcionament podem veure que quan passem la targeta blanca s'encén el led verd i el display mostra *Accés Autoritzat*. En canvi quan passo el clauer blau s'encén el led vermell i el dislpay mostra *Accés denegat*, ja que no reconeix aquest ID i per tant li denega l'accés. 
[Vídeo funcionament](https://drive.google.com/file/d/1XtuoIjQruhApgoQKQVsTtH0f0tIz9kit/view?usp=sharing)

## Exercici per pujar nota
Aquest mateix exercici és el que he realitzat en el projecte final junt amb l'Àlex Mata.
El hardware l'hem resolt amb la següent assignació de pins:
```cpp
// pins SD i RFID
#define SS_PIN_RFID 5
#define CS_PIN_SD 15
#define RST_PIN_RFID 17
// SCK RFID I SCK SD pin18
// MOSI RFID I MOSI SD pin23
// MISO RFID I MISO SD pin19 

// pins Display
#define SDA_PIN 21
#define SCL_PIN 22
```
Cal dir que en el pin del MISO del lector de SD hem hagut de conectar una resistència en sèrie. El lector de targetes RFID va conectat a 3.3V mentre que el adaptor de targetes Micro SD va a 5V. 

Pel que fa als leds els hem conectat als següents pins: 
```cpp
#define LED_vermell 2
#define LED_verd 0
#define LED_espera 4
#define LED_funciona 16
```
Com ja hem explicat en el projecte hem fet una web asíncrona que mostra el perfil d'una persona quan es passa una targeta. Ho hem fet a través d'una base de dades guardada a la targeta micro SD, on s'associa cada targeta amb una persona. 
#include "HX711.h" 
#include <SoftwareSerial.h> // Bluetooth TX RX için yazılım kütüphanesi 
HX711 agirlikSensoru;     // LoadCellin tanımlanması ve pinleri
const int loadCellDT= 1;
const int loadCellSCK= 0;
int loadCellKalibrasyonFaktoru = 380;
float veri;


int bluetoothTx = 0; // bluetooth tx 10 numaralı pindedir
int bluetoothRx = 1; // bluetooth rx 11 numaralı pindedir
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);


#include <Servo.h>
Servo servo_60;   // Servonun tanımlanması ve pini
const int servoPin = 10;  

const int sikistirmaStepPin = 11;
const int sikistirmaDirPin = 12;    // Sıkıştırma stepinin pinleri
const int sikistirmaEnPin = 13;

const int tamburStepPin = 11;
const int tamburDirPin = 12;   // Tambur stepinin pinleri
const int tamburEnPin = 13;

const int kasikStepPin = 11;
const int kasikDirPin = 12;    // Kaşık stepinin pinleri
const int kasikEnPin = 13;

const int enduktifSensorKasik1 = A0;  // Kaşık step için endüktif sensorlerin pinleri
const int enduktifSensorKasik2 = A1;  

const int enduktifSensorSikistirma1 = A2;   // Sıkıştırma step için endüktif sensorlerin pinleri
const int enduktifSensorSikistirma2 = A3;

const int infraredSensorTambur = A4;   // Tambur step için infrared sensör pini

const int makaronTutucu = 0;    // Selenoid sensör pini

int tamburStepHizi = 250;   // Hızı artırmak için değeri azalt.(Alt sınır = 50)
int kasikStepDonmeHizi = 250;   // Hızı artırmak için değeri azalt.(Alt sınır = 50)
int kasikStepVurmaHizi = 250;   // Hızı artırmak için değeri azalt.(Alt sınır = 50)
int sikistirmaStepDonmeHizi = 250; // Hızı artırmak için değeri azalt.(ALt sınır = 50)

int sikilikAyari = 0; // Bu değer kullanıcı tarafından girilecek olan tütünün sıkılık miktarıdır.(gr)


void setup() {
  
  bluetooth.begin(9600); // Bluetooth haberleşmesini başlat
  
  pinMode(sikistirmaStepPin,OUTPUT);
  pinMode(sikistirmaDirPin,OUTPUT);
  pinMode(sikistirmaEnPin,OUTPUT);    // Sıkıştırma stepi aktif hale getirildi.
  digitalWrite(sikistirmaEnPin,LOW);
  pinMode(enduktifSensorSikistirma1,INPUT);
  pinMode(enduktifSensorSikistirma2,INPUT);
  
  pinMode(tamburStepPin,OUTPUT);
  pinMode(tamburDirPin,OUTPUT);
  pinMode(tamburEnPin,OUTPUT);    // Tambur stepi aktif hale getirildi.
  digitalWrite(tamburDirPin,HIGH);
  digitalWrite(tamburEnPin,LOW);
  pinMode(infraredSensorTambur,INPUT);

  pinMode(kasikStepPin,OUTPUT);
  pinMode(kasikDirPin,OUTPUT);
  pinMode(kasikEnPin,OUTPUT);     // Kaşık stepi aktif hale getirildi.
  digitalWrite(kasikEnPin,LOW);
  pinMode(enduktifSensorKasik1,INPUT);
  pinMode(enduktifSensorKasik2,INPUT);

  pinMode(makaronTutucu ,OUTPUT);
  digitalWrite(makaronTutucu,LOW);
  
  servo_60.attach(servoPin);
  
  for (int pos = 90; pos >= 0; pos--) {    // Servo başlangıç konumuna getirildi. (0-180 aralığında 0'a getirildi)
    servo_60.write(pos);              
    delay(15);                  
  }


  while(digitalRead(infraredSensorTambur) == LOW) {    // Tambur başlangıç konumuna gelene kadar stepe dönme emri verir.(Mesneviden ders alınmadı!)
    digitalWrite(tamburStepPin,HIGH);
    delayMicroseconds(tamburStepHizi);             
    digitalWrite(tamburStepPin,LOW);
    delayMicroseconds(tamburStepHizi);
  }

  digitalWrite(kasikDirPin, LOW);      // Kaşığı çekecek yön bilgisi.
  
  while(digitalRead(enduktifSensorKasik2) == LOW) {       // Endüktif sensor kaşıktaki metal plakayı görene kadar stepe dön komutu verir.
    digitalWrite(kasikStepPin,HIGH);
    delayMicroseconds(kasikStepDonmeHizi);
    digitalWrite(kasikStepPin,LOW);
    delayMicroseconds(kasikStepDonmeHizi);
  }

  agirlikSensoru.begin(loadCellDT, loadCellSCK);      // LoadCell aktif hale getirildi.
  agirlikSensoru.tare();
  agirlikSensoru.tare();
  agirlikSensoru.set_scale(loadCellKalibrasyonFaktoru);

  digitalWrite(sikistirmaDirPin,LOW);      // Sıkıştırma plakasını çekecek yön bilgisi.

  while(digitalRead(enduktifSensorSikistirma2) == LOW) {       // Endüktif sensor kaşıktaki metal plakayı görene kadar stepe dön komutu verir.
    digitalWrite(sikistirmaStepPin, HIGH);
    delayMicroseconds(sikistirmaStepDonmeHizi);
    digitalWrite(sikistirmaStepPin,LOW);
    delayMicroseconds(sikistirmaStepDonmeHizi);
  }
 
}
void loop() { 
 servoDon();
 makaronIstiyimGelBeniAl();
 tutunSikistir();
 kasikStepDon();
 tutunSikistirmaPlakasiniGevset();
 makaronBenimKimseyeVermem();
 kasikStepVur();
 tamburStepDon();
}

void tamburStepDon(){    // 1/4 döndürmeye sahip.

  for(int x = 0; x < 500; x++) {
    digitalWrite(tamburStepPin,HIGH);
    delayMicroseconds(tamburStepHizi);    // Sensörü durma alanından çıkarana kadar stepi çalıştıran kod.
    digitalWrite(tamburStepPin,LOW);
    delayMicroseconds(tamburStepHizi);
  }
  
      
  while(digitalRead(infraredSensorTambur) == LOW) {     // Tambur durma konumuna gelene kadar, sensör stepe dönme emri verir.
    digitalWrite(tamburStepPin,HIGH);
    delayMicroseconds(tamburStepHizi);             
    digitalWrite(tamburStepPin,LOW);
    delayMicroseconds(tamburStepHizi);
  }
  
}

void servoDon(){   // servoyu ileri geri yapar(luk luk)
  servo_60.write(180);
  delay(100);
  servo_60.write(0);
}

void kasikStepDon(){
  
  digitalWrite(kasikDirPin,HIGH);     // Kaşığı itecek yön bilgisi.
  
  while(digitalRead(enduktifSensorKasik1) == LOW) {     // Endüktif sensor kaşıktaki metal plakayı görene kadar stepe dön komutu verir.
    digitalWrite(kasikStepPin,HIGH);
    delayMicroseconds(kasikStepDonmeHizi);
    digitalWrite(kasikStepPin,LOW);
    delayMicroseconds(kasikStepDonmeHizi);
  }

  delay(500);
  
  digitalWrite(kasikDirPin,LOW);      // Kaşığı çekecek yön bilgisi.
  
  while(digitalRead(enduktifSensorKasik2) == LOW) {       // Endüktif sensor kaşıktaki metal plakayı görene kadar stepe dön komutu verir.
    digitalWrite(kasikStepPin,HIGH);
    delayMicroseconds(kasikStepDonmeHizi);
    digitalWrite(kasikStepPin,LOW);
    delayMicroseconds(kasikStepDonmeHizi);
  }
}

void kasikStepVur(){
     
  digitalWrite(kasikDirPin,HIGH);     // Kaşığı itecek yön bilgisi.
  
  for(int x = 0; x < 5000; x++) {
    digitalWrite(kasikStepPin,HIGH);
    delayMicroseconds(kasikStepVurmaHizi);
    digitalWrite(kasikStepPin,LOW);
    delayMicroseconds(kasikStepVurmaHizi);
  }

  delay(500);
  
  digitalWrite(kasikDirPin,LOW);      // Kaşığı çekecek yön bilgisi.

  while(digitalRead(enduktifSensorKasik2) == LOW) {       // Endüktif sensor kaşıktaki metal plakayı görene kadar stepe dön komutu verir.
    digitalWrite(kasikStepPin,HIGH);
    delayMicroseconds(kasikStepDonmeHizi);
    digitalWrite(kasikStepPin,LOW);
    delayMicroseconds(kasikStepDonmeHizi);
  }
}

void tutunSikistir(){

 digitalWrite(sikistirmaDirPin, HIGH);      // Sıkıştırma plakasını itecek yön bilgisi.
 
 while(!(veri > sikilikAyari) && digitalRead(enduktifSensorSikistirma1) == LOW){      // LoadCelle belirtilen değer kadar baskı uygulayana veya 
    digitalWrite(sikistirmaStepPin,HIGH);                                             // endüktif sensör plakayı görene kadar step motor çalışır.
    delayMicroseconds(sikistirmaStepDonmeHizi);
    digitalWrite(sikistirmaStepPin,LOW);
    delayMicroseconds(sikistirmaStepDonmeHizi);
 }
}

void tutunSikistirmaPlakasiniGevset(){
  
  digitalWrite(sikistirmaDirPin,LOW);      // Sıkıştırma plakasını çekecek yön bilgisi.

  while(digitalRead(enduktifSensorSikistirma2) == LOW) {       // Endüktif sensor kaşıktaki metal plakayı görene kadar stepe dön komutu verir.
    digitalWrite(sikistirmaStepPin, HIGH);
    delayMicroseconds(sikistirmaStepDonmeHizi);
    digitalWrite(sikistirmaStepPin,LOW);
    delayMicroseconds(sikistirmaStepDonmeHizi);
  }
}

void makaronIstiyimGelBeniAl(){
  digitalWrite(makaronTutucu, HIGH);
}

void makaronBenimKimseyeVermem(){
  digitalWrite(makaronTutucu, LOW);
}

float loadcellkalibrasyon(){
 veri=0;
 for(int i = 0; i<12 ; i++){
  veri += agirlikSensoru.get_units();
 }
 veri = veri / 12;
  return veri;
}



void bluetoothBaglantisi(char deger){  // bu fonksiyona girmek için ayar fonksiyonu her loop beklenmeli eğer varsa sıkıştırma kontrol fonksiyonuna girmesi için uygun değer beklenmeli eğer deger varsa bu alt fonksiyona inebilir ayarları kaydet dediğinde ilgili ayar eeproma yazılabilir....
  
  while(bluetooth.available()) // bluetooth bağlantısından veri geliyorsa
  {
    if(bluetooth.readString()== "AYAR")   // alınan veriyi menu seceneği olarak al
      {
      bluetooth.write("t");
      bluetooth.write(tamburStepHizi);

      bluetooth.write("d");
      bluetooth.write(kasikStepDonmeHizi); 

      bluetooth.write("v");
      bluetooth.write(kasikStepVurmaHizi);   

      bluetooth.write("s");
      bluetooth.write(sikistirmaStepDonmeHizi);

      bluetooth.write("f");
      bluetooth.write(sikilikAyari);
      
      delay (100);
      switch(deger){
        
      case 'T+' :
         tamburDirPin=HIGH;
         for(int x = 0; x < 25; x++) {
         digitalWrite(tamburStepPin,HIGH);
         delayMicroseconds(tamburStepHizi);
         digitalWrite(tamburStepPin,LOW);
         delayMicroseconds(tamburStepHizi);
         }
      break;

      case 'T-' :
         tamburDirPin=LOW;
         for(int x = 0; x < 25; x++) {
         digitalWrite(tamburStepPin,HIGH);
         delayMicroseconds(tamburStepHizi);
         digitalWrite(tamburStepPin,LOW);
         delayMicroseconds(tamburStepHizi);
         }   
      break;

        case 'd':
        delay(100);
        kasikStepDonmeHizi=bluetooth.read();
        break;            

        case 'v':
        delay(100);
        kasikStepVurmaHizi=bluetooth.read();
        break;

        case 's':
        delay(100);
        sikistirmaStepDonmeHizi=bluetooth.read();
        break;

        case 'f':
        delay(100);
        sikilikAyari=bluetooth.read();
        break;            
      
      }
    
   }
}
return;

}

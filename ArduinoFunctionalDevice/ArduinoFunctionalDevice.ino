#include "DHT.h" //DHT sensörleri kitaplığını taslağımıza dahil ediyoruz.
#include <SFE_BMP180.h> //Basınç sensörü kitaplığını taslağımıza dahil ediyoruz.
#include <Wire.h> //Wire kütüphanesini taslağımıza dahil ediyoruz.
#include <LCD5110_Graph.h> //5110 grafik kütüphanesini taslağımıza dahil ediyoruz.
#include <Adafruit_BMP085.h>
#include <Servo.h>

#define DHTPIN 2 //DHT22'nin veri pinini Arduino'daki hangi pine bağladığımızı belirtiyoruz.
#define DHTTYPE DHT11 //Hangi DHT sensörünü kullandığımızı belirtiyoruz.
#define ALTITUDE 4.0 //Bulunduğunuz şehrin rakım değeri. Ben Eskişehir'deyim, yani 788. 
//Yaşadığınız şehrin rakımına buradan bakın: http://tr.wikipedia.org/wiki/T%C3%BCrkiye_il_merkez_rak%C4%B1mlar%C4%B1

DHT dht(DHTPIN, DHTTYPE); //Belirttiğimiz değişkenleri kaydettiriyoruz.
LCD5110 myGLCD(8,9,10,11,12); //5110 LCD'nin takılı olduğu pinleri söylüyoruz.
SFE_BMP180 bmp180;
Adafruit_BMP085 bmp;

Servo srv;

extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];
extern uint8_t TinyFont[];
extern uint8_t nem_bitmap[];
extern uint8_t sicaklik_bitmap[];
extern uint8_t intro[];
extern uint8_t basinc_bitmap[];
extern uint8_t yagmurlu[];
extern uint8_t gunesli[];
extern uint8_t karli[];
extern uint8_t uparrow[];
extern uint8_t downarrow[];
extern uint8_t equal[];

int gecenzaman=0;
int basinckarsilastirma[24] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //İçinde 24 adet değişken saklayan bir Array yaratıyoruz. Bu değişkenlerin her biri saat başı basınç değerlerini tutacak.
int firsttime =0; //Mini Hava İstasyonu'nun ilk defa açılıp açılmadığını kontrol etmek için oluşturduğumuz değişken.
const int switchpin = 6; //Switch (anahtar)ı bağladığımız pin 6.
const int button = 4;
const int komp = 5;
int toplam=0;
int ortalama=0;
int fark=0;
int tahmin;
int yonelim;
int basinc;
int mod = 0;
int aci;
int komppwm;

int harf;

int metbpm;
float metfreq;
float metmilisec;

float rakimF;

String Not;

void setup(){
  srv.attach(3);
  pinMode(switchpin, INPUT);
  pinMode(A3, INPUT);
  pinMode(A2, INPUT);
  pinMode(button, INPUT);
  pinMode(komp, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  dht.begin(); //DHT22 ısı ve nem sensörünü ölçümlere başlattırıyoruz.
  bmp180.begin(); //  BMP180 basınç sensörünü ölçümlere başlattırıyoruz.
  myGLCD.InitLCD(); //LCD başlatılıyor.
  myGLCD.setContrast(60); //Kontrast değeri belirleniyor.
  myGLCD.setFont(TinyFont);
  if (!bmp.begin()) {
  myGLCD.print("Bmp hata", CENTER, 8);
  while (1) {}
  }
  
}

//En başta girdiğimiz rakım bilgisini kullanarak, ham basınç değerleri deniz seviyesi basınç değerlerine dönüştürülüyor.
float readPressure()
{
  char status;
  double T,P,p0,a;

  status = bmp180.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = bmp180.getTemperature(T);
    if (status != 0)
    { 
      status = bmp180.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = bmp180.getPressure(P,T);
        if (status != 0)
        {
          p0 = bmp180.sealevel(P,ALTITUDE);       
          return p0;
        }
      }
    }
  }
}

void loop() {
  
int nem = dht.readHumidity(); //DHT22 sensöründen nem okunuyor ve nem değişkenine yazılıyor.
int sicaklik = dht.readTemperature(); //DHT22 sensöründen sıcaklık okunuyor ve nem değişkenine yazılıyor.int basinc = readPressure(); //BMP180 sensöründen basınç okunuyor ve basinc değişkenine yazılıyor.
  
float sicaklikf = dht.readTemperature(); //Detaylı bir şekilde göstermek için bir de float (küsüratlı) şeklinde değişken yapıyoruz.
float nemf = dht.readHumidity();  //Detaylı bir şekilde göstermek için bir de float (küsüratlı) şeklinde değişken yapıyoruz.
float basincf = readPressure(); //Detaylı bir şekilde göstermek için bir de float (küsüratlı) şeklinde değişken yapıyoruz.


toplam = basinckarsilastirma[0] + basinckarsilastirma[1] + basinckarsilastirma[2] + basinckarsilastirma[3] + basinckarsilastirma[4] + basinckarsilastirma[5] + basinckarsilastirma[6] + basinckarsilastirma[7] + basinckarsilastirma[8] + basinckarsilastirma[9] + basinckarsilastirma[10] + basinckarsilastirma[11] + basinckarsilastirma[12] + basinckarsilastirma[13] + basinckarsilastirma[14] + basinckarsilastirma[15] + basinckarsilastirma[16] + basinckarsilastirma[17] + basinckarsilastirma[18] + basinckarsilastirma[19] + basinckarsilastirma[20] + basinckarsilastirma[21] + basinckarsilastirma[22] + basinckarsilastirma[23]; //Belirli aralıklarla ölçülen basınç değerlerini topluyoruz.
ortalama = toplam / 24; //Sonra 3'e bölerek ortalamasını almış oluyoruz.
fark = basinc - ortalama; //Daha sonra güncel basınç değerini hesaplamış olduğumuz ortalamadan çıkararak aradaki farkı buluyoruz.

rakimF = bmp.readAltitude(101325);

// DÜŞÜK BASINÇ UYARISI (KIRMIZI LED)
if ((fark) <=-4) {   //4 hPa'lık düşüş olursa. 
digitalWrite(komp, HIGH); //Kırmızı yanar.
tahmin=3;
}

//OTOMATİK LCD ARKAPLAN IŞIĞI
int parlaklik = analogRead(A3);
int pot = analogRead(A2);
pot = map(pot,0,1023,1023,0);
aci = map(pot,0,1023,0,180);
komppwm = map(pot,0,1023,0,255);
if (parlaklik <=175) {  //Eğer ortamda ışık yetersizse, 
digitalWrite(7, HIGH);  // 5110 ekranının arkaplan ışığını yak.
}
else {
digitalWrite(7, LOW); //  Yoksa söndür.
}

  if (digitalRead(6) == LOW) { //ÇALIŞMA MODUNU BELİRLE. SWITCH AÇIKSA NORMAL MODDA ÇALIŞ.
    myGLCD.disableSleep(); //Ekranın güç tasarrufu modunu kapatıyoruz.
    
  if (firsttime == 0) { //İlk açılışta intromuzu gösteriyoruz ve sensörlerin okunduğu bilgisini veriyoruz...
  //Aşağıda İntro Animasyonu var...
    myGLCD.clrScr();
    myGLCD.drawBitmap(0, 0, intro, 84, 48);
    for (int i=84; i>=7; i--) {
    myGLCD.setFont(TinyFont);
    myGLCD.print("ARDUINO ISLEVSEL CIHAZ", i, 37);
    delay(20);
    myGLCD.update();
    }
    
    for (int i=48; i>=43; i--) {
    myGLCD.clrScr();
    myGLCD.drawBitmap(0, 0, intro, 84, 48);
    myGLCD.setFont(TinyFont);
    myGLCD.print("ISLEVSEL CIHAZ", 7, 37);
    myGLCD.print("SENSORLER OKUNUYOR", 8, i);
    delay(50);
    myGLCD.update();
    }
    
    delay(5000); //Sensörlerin okunması için gerekli zamanı sağlıyoruz.
    firsttime= 1; // Hava İstasyonu'nun açıldığını ve introyu bir daha göstermeye gerek olmadığını belirtiyoruz.
  
  }
  else {
  }

  if (digitalRead(button) == HIGH) {
    delay(100);
    if (digitalRead(button) == HIGH) {
      if (mod < 5) {
        mod = mod + 1;
        delay(500);
      }
      else {
        mod = 0;
      }
    }
  }

  else if (mod == 0) {
  
    if (pot < 250) {
    
    //SICAKLIK GÖSTERİLİR...
      myGLCD.clrScr();
      myGLCD.drawBitmap(0, 0, sicaklik_bitmap, 84, 48);
      myGLCD.setFont(BigNumbers);
      myGLCD.printNumF(sicaklikf,1,12 ,18);
      myGLCD.setFont(SmallFont);
      myGLCD.print("C",72,27);
      myGLCD.setFont(TinyFont);
      myGLCD.print("o",70,22);
      myGLCD.update();
      delay(100);
    }

    else if (pot < 500) {
      
    // NEM GÖSTERİLİR...
      myGLCD.clrScr();
      myGLCD.drawBitmap(0, 0, nem_bitmap, 84, 48);
      myGLCD.setFont(BigNumbers);
      myGLCD.printNumF(nemf,1,20, 20);
      myGLCD.setFont(SmallFont);
      myGLCD.print("%",9,28);
      myGLCD.setFont(TinyFont);
      myGLCD.update();
      delay(100);
    }

    else if (pot < 750) {
      
    // BASINÇ GÖSTERİLİR...   
  
      myGLCD.clrScr();
      myGLCD.drawBitmap(0, 0, basinc_bitmap, 84, 48);
      myGLCD.setFont(BigNumbers);
      myGLCD.printNumF(basincf,1,0,18);
      myGLCD.setFont(TinyFont);
      myGLCD.print("hPa",40,43);
      myGLCD.update();
      delay(100);
    }

    else if (pot < 1024) {

      // RAKIM GÖSTERİLİR...
      
      myGLCD.clrScr();
      myGLCD.setFont(SmallFont);
      myGLCD.print("RAKIM",CENTER,0);
      myGLCD.setFont(BigNumbers);
      myGLCD.printNumF(rakimF,1,CENTER,8);
      myGLCD.setFont(TinyFont);
      myGLCD.print("metre",CENTER,36);
      myGLCD.update();
      delay(100);
    }
  }
  else if (mod == 1) {
    //servo modu
    myGLCD.clrScr();
    myGLCD.setFont(SmallFont);
    myGLCD.print("Servo Kontrol",CENTER,0);
    myGLCD.setFont(BigNumbers);
    myGLCD.printNumI(aci,CENTER,8);
    myGLCD.setFont(TinyFont);
    myGLCD.print("derece",CENTER,36);
    myGLCD.update();
    srv.write(aci);
    delay(5);
  }
  else if (mod == 2) {
    //dıs komponent kontrol modu
    myGLCD.clrScr();
    myGLCD.setFont(SmallFont);
    myGLCD.print("Eleman Kontrol",CENTER,0);
    myGLCD.setFont(BigNumbers);
    myGLCD.printNumI(komppwm,CENTER,8);
    myGLCD.setFont(TinyFont);
    myGLCD.print("/255",CENTER,36);
    myGLCD.update();
    analogWrite(komp, komppwm);
  }
  else if (mod == 3) {
    // metin okuma modu
    myGLCD.clrScr();
    myGLCD.setFont(SmallFont);
    myGLCD.print("Metin:",CENTER,0);
    harf = map(pot,0,1023,0,20);
    if (harf == 0) {
      Not = "hey!..";
    }
    else if (harf == 1) {
      Not = "merhaba,";
    }
    else if (harf == 2) {
      Not = "simdi";
    }    
    else if (harf == 3) {
      Not = "sana";
    }
    else if (harf == 4) {
      Not = "bir";
    }
    else if (harf == 5) {
      Not = "hikaye";
    }
    else if (harf == 6) {
      Not = "anlatacagim...";
    }
    else if (harf == 7) {
      Not = "ama";
    }
    else if (harf == 8) {
      Not = "bu";
    }
    else if (harf == 9) {
      Not = "hikaye";
    }
    else if (harf == 10) {
      Not = "pek";
    }
    else if (harf == 11) {
      Not = "de";
    }
    else if (harf == 12) {
      Not = "eglenceli";
    }
    else if (harf == 13) {
      Not = "olmayabilir...";
    }
    else if (harf == 14) {
      Not = "neyse";
    }
    else if (harf == 15) {
      Not = "yine";
    }
    else if (harf == 16) {
      Not = "de";
    }
    else if (harf == 17) {
      Not = "anlatayim,";
    }
    else if (harf == 18) {
      Not = "derken";
    }
    else if (harf == 19) {
      Not = "sayfa";
    }
    else if (harf == 20) {
      Not = "bitti ya...";
    }
    myGLCD.setFont(TinyFont);
    myGLCD.print(Not,CENTER,16);
  }
  else if (mod == 4) {
    // Extra Metronom Modu: 5/08/2018 tarihinde Ahmet Akif KAYA tarafıdan servo ile metronom projesi için eklenmiştir.
    myGLCD.clrScr();
    myGLCD.setFont(SmallFont);
    myGLCD.print("Metronom:",CENTER,0);
    metbpm = map(pot,0,1023,40,218);
    metfreq = float(metbpm)/60.0;
    metmilisec = 1000.0/metfreq;
    myGLCD.setFont(BigNumbers);
    myGLCD.printNumI(metbpm,CENTER,8);
    myGLCD.setFont(TinyFont);
    myGLCD.print("/BPM",CENTER,36);
    srv.write(108);
    delay(metmilisec);
    srv.write(120);
    delay(metmilisec);
  }
}
}



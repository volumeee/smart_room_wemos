#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


#define DHTPIN D3        // Pin data sensor DHT11 terhubung ke pin D4 pada Wemos R1 D1 Mini
#define DHTTYPE DHT11    // Jenis sensor DHT
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "POCO";
const char* password = "12345678";
const char* blynkAuth = "MvHQIqnfqUm7XW3UBuQQZdgQ260ww-A8";
const char* blynkTemplateId = "TMPLaUmi5_J6";

BlynkTimer timer; //merubah perintah BlynkTimer menjadi "timer"

const int ledPin = D5;    // Pin LED terhubung ke pin D1 pada Wemos R1 D1 Mini
const int buzzerPin = D8; // Pin buzzer terhubung ke pin D3 pada Wemos R1 D1 Mini
bool buzzerFlag = true;
bool kirimDataAktif = false; // Variabel flag untuk mengaktifkan/mematikan pengiriman data

void kirimData(){ //void untuk mengirimkan pembacaan sensor atau nilai ke blynk
  if (kirimDataAktif == false) {
    // Kode pengiriman data ke Blynk
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  Blynk.virtualWrite(V1, temperature); //mengirimkan data suhu ke virtual pin1 pada blynk
  Blynk.virtualWrite(V2, humidity); //mengirimkan data suhu ke virtual pin1 pada blynk
  
  Serial.print("Temp: " + String(temperature, 2) + "C"); //menampilkan tulisan Temp: nilai suhu dhtC ke dalam serial monitor
  Serial.print(" | ");
  Serial.println("Humi: " + String(humidity, 2) + "RH"); //menampilkan tulisan Temp: nilai suhu dhtC ke dalam serial monitor
  Serial.println("------");
  
  if (temperature > 30) {   //kondisi atau perintah apabila suhu -0 maka akan melakukan perintah sesuai kondisi yg ditentukan dibawa
    digitalWrite(ledPin, HIGH);   // Menghidupkan LED
    if (buzzerFlag == true){
        Serial.println("Suhu Lebih Dari 30 Derajat Maka AC Menyala");
        Blynk.virtualWrite(V0, 1); //perintah untuk memasukkan nilai ke pin virtual blynk
        digitalWrite(buzzerPin, HIGH);// Menghidupkan buzzer
        delay(1000);                  // Membunyikan buzzer selama 1 detik
        digitalWrite(buzzerPin, LOW); // Mematikan buzzer
        delay(1000);                  // Membunyikan buzzer selama 1 detik
        buzzerFlag = false;
    }

  }
  else{ 
    
    if (buzzerFlag == false){
        Serial.println("Suhu kurang dari 30 Derajat terlalu dingin AC dimatikan otomatis");
        Blynk.virtualWrite(V0, 0); //perintah untuk memasukkan nilai ke pin virtual blynk
        digitalWrite(ledPin, LOW);    // Mematikan LED
        digitalWrite(buzzerPin, HIGH);// Menghidupkan buzzer
        delay(1000);                  // Membunyikan buzzer selama 1 detik
        digitalWrite(buzzerPin, LOW); // Mematikan buzzer
        delay(1000);                  // Membunyikan buzzer selama 1 detik
        buzzerFlag = true;
    }
  }
  kirimDataAktif == false;
  }

}

BLYNK_WRITE(V0){ //perintah untuk membaca nilai yang dikirim oleh blynk
  int pinVal = param.asInt(); //perintah untuk memasukkan nilai dari pin virtual blynk ke variabel pinVal
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (pinVal == 1) //kondisi jika pinval sama dengan 1 maka LED1 menyala 
    {
      digitalWrite(ledPin, HIGH);   // Menghidupkan LED
      kirimDataAktif = false; // Matikan pengiriman data jika nilai = 0
      Serial.println("AC nyala Manual");
      if (buzzerFlag == true){
          Serial.println("Suhu Lebih Dari 30 Derajat Maka AC Menyala");
          Blynk.virtualWrite(V0, 1); //perintah untuk memasukkan nilai ke pin virtual blynk
          digitalWrite(buzzerPin, HIGH);// Menghidupkan buzzer
          delay(1000);                  // Membunyikan buzzer selama 1 detik
          digitalWrite(buzzerPin, LOW); // Mematikan buzzer
          delay(1000);                  // Membunyikan buzzer selama 1 detik
          buzzerFlag = false;
      }
    }
  else if(pinVal == 0)
    {
      digitalWrite(ledPin, LOW);   // Menghidupkan LED
      kirimDataAktif = true; // Hidupkan pengiriman data jika nilai != 0
      Serial.println("AC mati Manual");
        if (buzzerFlag == false){
            Blynk.virtualWrite(V0, 0); //perintah untuk memasukkan nilai ke pin virtual blynk
            digitalWrite(ledPin, LOW);    // Mematikan LED
            digitalWrite(buzzerPin, HIGH);// Menghidupkan buzzer
            delay(1000);                  // Membunyikan buzzer selama 1 detik
            digitalWrite(buzzerPin, LOW); // Mematikan buzzer
            delay(1000);                  // Membunyikan buzzer selama 1 detik
            buzzerFlag = true;
        }
    }
}

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  dht.begin();
  Blynk.begin(blynkAuth, ssid, password);
  timer.setInterval(2000, kirimData); //interval pengiriman data dari mikrokontroler ke blynk
}

void loop() {
  Blynk.run(); //menjalankan perintah yang ada di library blynk
  timer.run(); //menjalankan timer dari blynk
}

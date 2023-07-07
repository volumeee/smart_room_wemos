#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define DHTPIN D3     // Pin data sensor DHT11 terhubung ke pin D4 pada Wemos R1 D1 Mini
#define DHTTYPE DHT11 // Jenis sensor DHT
DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "POCO";
const char *password = "123456789";
const char *blynkAuth = "MvHQIqnfqUm7XW3UBuQQZdgQ260ww-A8";
const char *blynkTemplateId = "TMPLaUmi5_J6";

BlynkTimer timer; // Merubah perintah BlynkTimer menjadi "timer"

const int kipasPin = D5;  // Pin LED terhubung ke pin D1 pada Wemos R1 D1 Mini
const int buzzerPin = D8; // Pin buzzer terhubung ke pin D3 pada Wemos R1 D1 Mini
bool buzzerFlag = true;
bool kirimDataAktif = false; // Variabel flag untuk mengaktifkan/mematikan pengiriman data

void kirimData()
{ // Void untuk mengirimkan pembacaan sensor atau nilai ke Blynk
  if (kirimDataAktif == true)
  {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    Blynk.virtualWrite(V1, temperature); // Mengirimkan data suhu ke virtual pin 1 pada Blynk
    Blynk.virtualWrite(V2, humidity);    // Mengirimkan data kelembaban ke virtual pin 2 pada Blynk

    Serial.print("Temp: " + String(temperature, 2) + "C"); // Menampilkan tulisan Temp: nilai suhu dhtC ke dalam serial monitor
    Serial.print(" | ");
    Serial.println("Humi: " + String(humidity, 2) + "RH"); // Menampilkan tulisan Humi: nilai kelembaban dhtC ke dalam serial monitor
    Serial.println("------");

    if (temperature > 30)
    {                               // Kondisi atau perintah apabila suhu lebih dari 30 derajat Celsius
      digitalWrite(kipasPin, HIGH); // Menghidupkan LED

      if (buzzerFlag == true)
      {
        Serial.println("Suhu Lebih Dari 30 Derajat Maka AC Menyala");
        // Blynk.virtualWrite(V0, 1); // Perintah untuk memasukkan nilai ke pin virtual Blynk
        digitalWrite(buzzerPin, HIGH); // Menghidupkan buzzer
        delay(1000);                   // Membunyikan buzzer selama 1 detik
        digitalWrite(buzzerPin, LOW);  // Mematikan buzzer
        delay(1000);                   // Membunyikan buzzer selama 1 detik
        buzzerFlag = false;
      }
    }
    else
    {
      if (buzzerFlag == false)
      {
        Serial.println("Suhu kurang dari 30 Derajat terlalu dingin AC dimatikan otomatis");
        // Blynk.virtualWrite(V0, 0); // Perintah untuk memasukkan nilai ke pin virtual Blynk
        digitalWrite(kipasPin, LOW);   // Mematikan LED
        digitalWrite(buzzerPin, HIGH); // Menghidupkan buzzer
        delay(1000);                   // Membunyikan buzzer selama 1 detik
        digitalWrite(buzzerPin, LOW);  // Mematikan buzzer
        delay(1000);                   // Membunyikan buzzer selama 1 detik
        buzzerFlag = true;
      }
    }
  }
}

BLYNK_WRITE(V0)
{ // Saklar manual/otomatis
  int pinVal = param.asInt();

  if (pinVal == 1)
  {
    kirimDataAktif = true;
    Serial.println("Mode: Otomatis Menyala");
  }
  else if (pinVal == 0)
  {
    kirimDataAktif = false;
    Serial.println("Mode: Otomatis dimatikan ");
  }
}

BLYNK_WRITE(V3)
{ // Saklar on/off manual
  int pinVal = param.asInt();

  if (pinVal == 1)
  {
    digitalWrite(kipasPin, HIGH); // Menghidupkan LED
    Serial.println("Kipas: Nyala Manual");

    if (buzzerFlag == true)
    {
      // Blynk.virtualWrite(V0, 1); // Perintah untuk memasukkan nilai ke pin virtual Blynk
      digitalWrite(buzzerPin, HIGH); // Menghidupkan buzzer
      delay(1000);                   // Membunyikan buzzer selama 1 detik
      digitalWrite(buzzerPin, LOW);  // Mematikan buzzer
      delay(1000);                   // Membunyikan buzzer selama 1 detik
      buzzerFlag = false;
    }
  }
  else if (pinVal == 0)
  {
    digitalWrite(kipasPin, LOW); // Mematikan LED
    Serial.println("Kipas: Mati Manual");

    if (buzzerFlag == false)
    {
      // Blynk.virtualWrite(V0, 0); // Perintah untuk memasukkan nilai ke pin virtual Blynk
      digitalWrite(kipasPin, LOW);   // Mematikan LED
      digitalWrite(buzzerPin, HIGH); // Menghidupkan buzzer
      delay(1000);                   // Membunyikan buzzer selama 1 detik
      digitalWrite(buzzerPin, LOW);  // Mematikan buzzer
      delay(1000);                   // Membunyikan buzzer selama 1 detik
      buzzerFlag = true;
    }
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(kipasPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  dht.begin();
  Blynk.begin(blynkAuth, ssid, password);
  timer.setInterval(2000, kirimData); // Interval pengiriman data dari mikrokontroler ke Blynk
}

void loop()
{
  Blynk.run(); // Menjalankan perintah yang ada di library Blynk
  timer.run(); // Menjalankan timer dari Blynk
}

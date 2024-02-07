//PIR Sensörden Telegrama Uyarı Mesajı Atma////////
//Beşikdüzü Fatih MTAL - 2024 /////////

#include <ESP8266WiFi.h>//ESP ile default geliyor yüklemeye gerek yok.#include <WiFiClientSecure.h> //ESP ile default geliyor yüklemeye gerek yok.
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> //https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/archive/master.zip
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

// Kablosuz Ağ Bilgilerinizi Girin
const char* ssid = "TTNET_ZTE_T321"; //1- WiFi adını yaz 
const char* password = "15E37jT321"; //2- Wifi şifreni yaz


// Telegram BOT için gereken token'ı girin
#define BOTtoken "6944022316:AAFCW5R8kRmOMvdqxAfWPOzsqV3sQgqp5_4"  // BotFather'dan aldığınız TOKEN

// IDBOT üzerinden /getid mesajı ile aldığımız ID bilgimizi gireceğiz
// Botun bizimle haberleşebilmesi için /start komutunu çalıştırmamız gerekli
#define CHAT_ID "6152621727" //3- IDBOTtan alınan id yazılacak!!!
X509List cert(TELEGRAM_CERTIFICATE_ROOT);

//telegram bota bağlantı için gerekli tanımlamaları yapmalıyız
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOTtoken, secured_client);

//PIR sensörünü bağlayacağımız pin numarasını ve hareket kontrol fonksiyonunu tanımlıyoruz
const int hareketSensoru = D1;
bool hareketAlgilandi = false;
int hareketvar=0;

// Hareket algılandığında çalışacak interrupt fonksiyonu
void IRAM_ATTR ISR() {
  Serial.println("HAREKET ALGILANDI!!!");
  hareketAlgilandi = true;
  hareketvar=1;
}

void setup() {
  Serial.begin(115200);

  // PIR sensörün bağlı olduğu pini tanımlıyoruz
  pinMode(hareketSensoru, INPUT_PULLUP);
  // Bu pini bir interrupt(kesme) olarak tanımlayalım ki herhangi bir anda kodu durdurup çalışabilsin. Modunu RISING olarak ayarlıyoruz.
  attachInterrupt(digitalPinToInterrupt(hareketSensoru), ISR, RISING);//yani sensör hareket algılayınca tespit fonk çalışacak

 // Kablosuz Ağa bağlanıyoruz
  Serial.print("Kablosuz Ağa Bağlanıyor: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  //bağlanana kadar bekletiyoruz
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Ağa Bağlanıldı");
  Serial.print("IP adresi: ");
  Serial.println(WiFi.localIP());
  //sistemin çalışmaya başladığını bot'a mesaj göndererek bildiriyoruz
  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot.sendMessage(CHAT_ID, "Bot started up", "");

}

void loop() {
  
  //eğer hareket algılanırsa
  if ((hareketAlgilandi) && hareketvar==1) {
    hareketvar++;
    //telegrama mesaj gönder
    bot.sendMessage(CHAT_ID, "Orman Yangını Tespit Edildi!!", "");
    Serial.println("Orman Yangını Tespit Edildi");
    hareketAlgilandi = false;//kullandığımız için tekrar resetleyelim ki tekrar kullanabilelim.
    
  }

}

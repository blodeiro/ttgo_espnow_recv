#include <esp_now.h>
#include <WiFi.h>

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "webserver.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

beehive_message beehiveSample;
bool dato_novo = false;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&beehiveSample, incomingData, sizeof(beehiveSample));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.printf("ID: %i\tT1: %f\tT2: %f\tH1: %f\tH2: %f\tP2: %f\tTe: %f\tHe: %f\n",
      beehiveSample.board_id, beehiveSample.temp_1, beehiveSample.temp_2, beehiveSample.hum_1, beehiveSample.hum_2,
      beehiveSample.pres_2, beehiveSample.temp_ext, beehiveSample.hum_ext);
  dato_novo = true;
}

void onTxDone() {
  Serial.println("TxDone");
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi connected");
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected: ");
  display.println(WiFi.localIP());
  display.print("Wi-Fi Channel: ");
  display.println(WiFi.channel());
  display.display();
  delay(5000);
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  static int retry = 0;
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.print("WiFi lost connection. Trying to reconnect: ");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Reconecting to WiFi: ");
  display.println(retry);
  display.display();
  WiFi.begin(ssid, password);
  retry++;
}

void setup_lora_board()
{
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LORA SENDER ");
  display.display();

  Serial.println("LoRa Sender Test");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  LoRa.onTxDone(onTxDone);

  Serial.println("LoRa Initializing OK!");
  display.setCursor(0, 10);
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}

void setup_wifi()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_AP_STA);

  WiFi.onEvent(WiFiStationConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
#include <Arduino.h>

#include "inits.h"


void lora_send()
{
  static int counter = 0;

  while (LoRa.beginPacket() == 0)
  {
    Serial.print("waiting for radio ... ");
    delay(100);
  }
  Serial.print("Sending packet: ");
  Serial.println(counter);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.write((uint8_t *)&beehiveSample, sizeof(beehiveSample));
  LoRa.endPacket(true);
  counter++;
}

void update_display()
{
  static int counter = 0;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("ESP DATA");
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.setCursor(70, 0);
  display.print("ID:");
  display.setCursor(100, 0);
  display.print(beehiveSample.board_id);

  display.setCursor(0, 15);
  display.print("Te:");
  display.setCursor(20, 15);
  display.print(beehiveSample.temp_ext);
  display.setCursor(70, 15);
  display.print("He:");
  display.setCursor(90, 15);
  display.print(beehiveSample.hum_ext);

  display.setCursor(0, 25);
  display.print("T1:");
  display.setCursor(20, 25);
  display.print(beehiveSample.temp_1);
  display.setCursor(70, 25);
  display.print("H1:");
  display.setCursor(90, 25);
  display.print(beehiveSample.hum_1);

  display.setCursor(0, 35);
  display.print("T2:");
  display.setCursor(20, 35);
  display.print(beehiveSample.temp_2);
  display.setCursor(70, 35);
  display.print("H2:");
  display.setCursor(90, 35);
  display.print(beehiveSample.hum_2);

  display.setCursor(0, 45);
  display.print("Dato:");
  display.setCursor(30, 45);
  display.print(counter);
  display.setCursor(70, 45);
  display.print("P2:");
  display.setCursor(90, 45);
  display.print(beehiveSample.pres_2);

  display.display();

  counter ++;
}


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  if (setCpuFrequencyMhz(80))
  {
    Serial.println("Frecuencia reducida");
  }
  else
  {
    Serial.println("Frecuencia normal");
  }
  setup_lora_board();
  setup_wifi();
  runserver();
}

void loop() {
  if (dato_novo)
  {
    //lora_send(); //TODO: Check WDT reset
    update_display();
    dato_novo = false;
  }
}
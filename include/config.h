
//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Loop configurations
#define F_PRINT 2000
#define F_BLE 2000
#define F_LORA 5000

// Message structure for unpacking espnow message
typedef struct beehive_message {
  int board_id;
  // SHT3X inside
  float temp_1;
  float hum_1;
  // BME280 inside
  float temp_2;
  float hum_2;
  float pres_2;
  // SHT3X outside
  float temp_ext;
  float hum_ext;
} beehive_message;
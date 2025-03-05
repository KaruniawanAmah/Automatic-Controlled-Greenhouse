#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h> // only for esp_wifi_set_channel()

// Global copy of slave
esp_now_peer_info_t slave;
#define CHANNEL 1
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Placeholder values
float temperature = 25.5;
float setTemp = 28.0;
float humidity = 60.2;
float setHumidity = 65.0;
bool mistingStatus = false;
/////////////////////button var////////////////////
#define buttonmode 12
#define buttonmode2 13
#define buttonmode3 14
bool bootup = true;
bool buttonmod = true;
bool buttonup = true;
bool buttondown = true;
int mode = 0;


// Init ESP Now with fallback
typedef struct __attribute__((packed)) struct_message {
  float temp;
  float humidity;
  bool misting;
} struct_message;

struct_message datatosend;
struct_message receivedData;
void setup() {
  datatosend.temp = 28.0;
  datatosend.humidity = 50.0;
  datatosend.misting = false;
  oledinit();
  Serial.begin(115200);
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
  Serial.println("ESPNow/Basic/Master Example");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  Serial.print("STA CHANNEL "); Serial.println(WiFi.channel());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

   // **Register Receive Callback**
  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
  // In the loop we scan for slave
  ScanForSlave();
  oleddisplay();
  // If Slave is found, it would be populate in `slave` variable
  // We will check if `slave` is defined and then we proceed further
  if (slave.channel == CHANNEL) { // check if slave channel is defined
    // `slave` is defined
    // Add slave as peer if it has not been added already
    bool isPaired = manageSlave();
    if (isPaired) {
      // pair success or already paired
      // Send data to device
      datatosend.temp = setTemp;
      datatosend.humidity = setHumidity;
      datatosend.misting = mistingStatus;
      sendData();
    } else {
      // slave pair failed
      Serial.println("Slave pair failed!");
    }
  }
  else {
    // No slave found to process
  }

  // wait for 3seconds to run the logic again
  delay(10);
}

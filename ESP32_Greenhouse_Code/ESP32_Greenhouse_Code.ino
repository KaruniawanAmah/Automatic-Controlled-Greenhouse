#include <esp_now.h>
#include <WiFi.h>
#define kipas1 0
#define kipas2 15
#define CHANNEL 1
#define relay 23
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
// Global copy of master
esp_now_peer_info_t master;
#define CHANNEL 1
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

typedef struct __attribute__((packed)) struct_message {
  float temp;
  float humidity;
  bool misting;
} struct_message;

// Create a global struct instance to store received data
struct_message receivedData;
struct_message sensorData;
// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
    Serial.print("AP CHANNEL "); Serial.println(WiFi.channel());
  }
}

void setup() {
  pinMode(relay, OUTPUT);
  pinMode(kipas1, OUTPUT);
  pinMode(kipas2, OUTPUT);
  Serial.begin(115200);
  dht.begin();
  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
}


void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  // Debugging: Check received data length
  Serial.print("Received Data Length: "); Serial.println(data_len);

  // Ensure the received data size matches the struct size
  if (data_len == sizeof(receivedData)) {
    memcpy(&receivedData, data, sizeof(receivedData));

    // Print the received values
    Serial.print("Last Packet Recv from: "); Serial.println(macStr);
    Serial.print("Temperature: "); Serial.println(receivedData.temp, 2);  // Print with 2 decimal places
    Serial.print("Humidity: "); Serial.println(receivedData.humidity, 2);
    Serial.println(receivedData.misting);
  } else {
    Serial.println("ERROR: Received data size mismatch!");
  }
  Serial.println("");
}

void loop() {
  // analogWrite(kipas1, 165);
  // analogWrite(kipas2, 165);
  system();
  fansystem();
 dhtcall();
 Serial.print(sensorData.temp);
 Serial.println(sensorData.humidity);
 if (master.channel == CHANNEL) { // check if slave channel is defined
    // `slave` is defined
    // Add slave as peer if it has not been added already
    bool isPaired = manageMaster();
    if (isPaired) {
      // pair success or already paired
      // Send data to device
      sensorData.misting = receivedData.misting;
      sendData();
    } else {
      // slave pair failed
      Serial.println("Slave pair failed!");
    }
  }
}

uint8_t data = 0;
// send data
void sendData() {
  // data++;
  
  const uint8_t *peer_addr = master.peer_addr;
  // Serial.print("Sending: "); Serial.println(sensorData);
  esp_err_t result = esp_now_send(peer_addr, (uint8_t*)&sensorData, sizeof(sensorData));
  // Serial.print("Send Status: ");
  // if (result == ESP_OK) {
  //   Serial.println("Success");
  // } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
  //   // How did we get so far!!
  //   Serial.println("ESPNOW not Init.");
  // } else if (result == ESP_ERR_ESPNOW_ARG) {
  //   Serial.println("Invalid Argument");
  // } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
  //   Serial.println("Internal Error");
  // } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
  //   Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  // } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
  //   Serial.println("Peer not found.");
  // } else {
  //   Serial.println("Not sure what happened");
  // }
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  // Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void ScanForMaster() {
  int16_t scanResults = WiFi.scanNetworks(false, false, false, 300, CHANNEL); // Scan only on one channel
  // reset on each scan
  bool masterFound = 0;
  memset(&master, 0, sizeof(master));

  Serial.println("");
  if (scanResults == 0) {
    // Serial.println("No WiFi devices in AP Mode found");
  } else {
    // Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      if (PRINTSCANRESULTS) {
        // Serial.print(i + 1);
        // Serial.print(": ");
        // Serial.print(SSID);
        // Serial.print(" (");
        // Serial.print(RSSI);
        // Serial.print(")");
        // Serial.println("");
      }
      delay(10);
      // Check if the current device starts with `Master`
      if (SSID.indexOf("master") == 0) {
        // SSID of interest
        // Serial.println("Found a Master.");
        // Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Master
        int mac[6];
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            master.peer_addr[ii] = (uint8_t) mac[ii];
          }
        }

        master.channel = CHANNEL; // pick a channel
        master.encrypt = 0; // no encryption

        masterFound = 1;
        // we are planning to have only one slave in this example;
        // Hence, break after we find one, to be a bit efficient
        break;
      }
    }
  }
}

bool manageMaster() {
  if (master.channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      deletePeer();
    }

    Serial.print("Slave Status: ");
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(master.peer_addr);
    if ( exists) {
      // Slave already paired.
      // Serial.println("Already Paired");
      return true;
    } else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(&master);
      if (addStatus == ESP_OK) {
        // Pair success
        // Serial.println("Pair success");
        return true;
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        // Serial.println("ESPNOW Not Init");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        // Serial.println("Invalid Argument");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        // Serial.println("Peer list full");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        // Serial.println("Out of memory");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        // Serial.println("Peer Exists");
        return true;
      } else {
        // Serial.println("Not sure what happened");
        return false;
      }
    }
  } else {
    // No slave found to process
    // Serial.println("No Slave found to process");
    return false;
  }
}

void deletePeer() {
  esp_err_t delStatus = esp_now_del_peer(master.peer_addr);
  // Serial.print("Master Delete Status: ");
  // if (delStatus == ESP_OK) {
  //   // Delete success
  //   Serial.println("Success");
  // } else if (delStatus == ESP_ERR_ESPNOW_NOT_INIT) {
  //   // How did we get so far!!
  //   Serial.println("ESPNOW Not Init");
  // } else if (delStatus == ESP_ERR_ESPNOW_ARG) {
  //   Serial.println("Invalid Argument");
  // } else if (delStatus == ESP_ERR_ESPNOW_NOT_FOUND) {
  //   Serial.println("Peer not found.");
  // } else {
  //   Serial.println("Not sure what happened");
  // }
}


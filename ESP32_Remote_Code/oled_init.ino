void oledinit() {
    // Serial.begin(115200);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.clearDisplay();

  pinMode(buttonmode,INPUT_PULLUP);
  pinMode(buttonmode2,INPUT_PULLUP);
  pinMode(buttonmode3,INPUT_PULLUP);
}

void oleddisplay(){

  buttonmod = digitalRead(buttonmode);
  buttonup = digitalRead(buttonmode3);
  buttondown = digitalRead(buttonmode2);
  if(bootup){
    oledprint();
    bootup = false;
  }
  if(!buttonmod){
    if(mode<2){
      mode++;
    }
    else{
      mode = 0;
    }
  }

  if(mode == 0){
    if(!buttonup){
      setTemp += 0.1;
      oledprint();
    }
    if(!buttondown){
      setTemp -=0.1;
      oledprint();
    }
  }
  if(mode == 1){
    if(!buttonup){
      setHumidity += 0.1;
      oledprint();
    }
    if(!buttondown){
      setHumidity -=0.1;
      oledprint();
    }
  }
  if(mode == 2){
    if(!buttonup){
      mistingStatus = true;
      oledprint();
    }
    if(!buttondown){
      mistingStatus = false;
      oledprint();
    }
  }
}

void oledprint(){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Temperature
    display.setCursor(0, 0);
    display.print("Temp:");  
    display.print(temperature, 1); 
    display.setCursor(65, 0);
    display.print("| Set:"); 
    display.print(setTemp, 1);

    // Humidity
    display.setCursor(0, 10);
    display.print("Humid:");
    display.print(humidity, 1);
    display.setCursor(65, 10);
    display.print("| Set:"); 
    display.print(setHumidity, 1);

    // Misting Status
    display.setCursor(0, 20);
    display.print("Misting: ");  
    display.print(mistingStatus ? "ON" : "OFF");

    display.display();
}
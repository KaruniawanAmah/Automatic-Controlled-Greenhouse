void system(){
  if(sensorData.temp < (receivedData.temp-0.2)){
    digitalWrite(relay, HIGH);
  }
  if(sensorData.temp > (receivedData.temp+0.5)){
    digitalWrite(relay, LOW);
  }
}

#define MAX_SPEED 170  // Max fan speed (0-170 for 0-5V)

// Weights for exhaust and intake
float K_tE = 100;  // Temperature weight for exhaust
float K_hE = 30;  // Humidity weight for exhaust

float K_tI = 70;  // Temperature weight for intake
float K_hI = -20; // Humidity weight for intake (negative means it increases when humidity is low)

void fansystem() {
  // Corrected error calculation
  float tempError = sensorData.temp - receivedData.temp;
  float humError = sensorData.humidity - receivedData.humidity;

  int exhaustSpeed = 0;
  int intakeSpeed = 0;

  // Only adjust fans if temp is above setpoint
  if (tempError > 0) {
    exhaustSpeed = (K_tE * tempError) + (K_hE * humError);
    intakeSpeed  = (K_tI * tempError) + (K_hI * humError);
  }

  if(exhaustSpeed<0){
    exhaustSpeed = 0;
  }
  if(intakeSpeed<0){
    intakeSpeed = 0;
  }
  // Constrain speeds to safe limits
  exhaustSpeed = constrain(exhaustSpeed, 0, MAX_SPEED);
  intakeSpeed  = constrain(intakeSpeed, 0, MAX_SPEED);

  // Apply speed to each fan
  analogWrite(kipas1, exhaustSpeed);
  analogWrite(kipas2, intakeSpeed);
}
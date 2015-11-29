const unsigned long THERMAL_CONTROL_SCR_TIMEOUT_MICROS = 100000;
const unsigned long THERMAL_CONTROL_SCR_EPS_MICROS = 32;

const byte TEMPERATURE_MIN = 0;
const byte TEMPERATURE_MAX = 255;

unsigned long thermalControlPulseLengthMax = 10 * 1000;

// public
void setupThermalControl();
void setTemperature(byte t);
void minimizeTemperature();
void maximizeTemperature();

// private
void resetThermalControl();
void findThermalControlPulseLengthMax();
void temperatureUp();
void temperatureDown();
void fixTemperature();
boolean isThermalControlLow();
boolean isThermalControlHigh();

void setupThermalControl() {
  pinMode(THERMAL_CONTROL_DOWN, OUTPUT);
  pinMode(THERMAL_CONTROL_UP, OUTPUT);
  pinMode(THERMAL_CONTROL_SCR, INPUT);
  resetThermalControl();
}

void resetThermalControl() {
  fixTemperature();
  #if SerialPID == true
    Serial.println("Thermal control > reset sequence started...");
  #endif
  findThermalControlPulseLengthMax();
}
void findThermalControlPulseLengthMax() {
  minimizeTemperature();
  #if SerialPID == true
    Serial.println("Thermal control > finding SCR pulse length max. value...");
  #endif
  temperatureUp();
  #if SerialPID == true
    Serial.println("Thermal control > Waiting for SCR to become high...");
  #endif
  thermalControlPulseLengthMax = 0;
  unsigned long thermalControlPulseLength;
  do {
    thermalControlPulseLength = pulseIn(THERMAL_CONTROL_SCR, HIGH, THERMAL_CONTROL_SCR_TIMEOUT_MICROS);
    if (thermalControlPulseLength > thermalControlPulseLengthMax) {
      thermalControlPulseLengthMax = thermalControlPulseLength;
    }
  } while ((thermalControlPulseLength > 0) || (digitalRead(THERMAL_CONTROL_SCR) != HIGH));
  #if SerialPID == true
    Serial.print("Thermal control > SCR pulse length max. value: ");
    Serial.print(thermalControlPulseLengthMax);
    Serial.println(" micros");
  #endif
  minimizeTemperature();
}

void setTemperature(byte t) {
  if (t == TEMPERATURE_MIN) {
    minimizeTemperature();
  } else if (t == TEMPERATURE_MAX) {
    maximizeTemperature();
  } else {
    #if SerialPID == true
      Serial.println("Thermal control > Updating temperature... ");
    #endif
    unsigned long targetPulseLength = ((unsigned long) t) * thermalControlPulseLengthMax / TEMPERATURE_MAX;
    unsigned long thermalControlPulseLength = pulseIn(THERMAL_CONTROL_SCR, HIGH, THERMAL_CONTROL_SCR_TIMEOUT_MICROS);
    while (abs(thermalControlPulseLength - targetPulseLength) > THERMAL_CONTROL_SCR_EPS_MICROS) {
      if (thermalControlPulseLength > targetPulseLength) {
        temperatureDown();
      } else {
        temperatureUp();
      }
      thermalControlPulseLength = pulseIn(THERMAL_CONTROL_SCR, HIGH, THERMAL_CONTROL_SCR_TIMEOUT_MICROS);
    }
    #if SerialPID == true
      Serial.print("Thermal control > New temperature: ");
      Serial.print(round(t * 100.0 / TEMPERATURE_MAX));
      Serial.println("%");
    #endif
    fixTemperature();
  }
}

void minimizeTemperature() {
  if (isThermalControlLow()) {
    return;
  }
  temperatureDown();
  #if SerialPID == true
    Serial.println("Thermal control > Waiting for SCR to become MIN...");
  #endif
  while (!isThermalControlLow()) {
    delay(100);
  }
  #if SerialPID == true
    Serial.println("Thermal control > SCR is MIN.");
  #endif
  fixTemperature();
}

void maximizeTemperature() {
  if (isThermalControlHigh()) {
    return;
  }
  temperatureUp();
  #if SerialPID == true
    Serial.println("Thermal control > Waiting for SCR to become MAX...");
  #endif
  while (!isThermalControlHigh()) {
    delay(100);
  }
  #if SerialPID == true
    Serial.println("Thermal control > SCR is MAX.");
  #endif
  fixTemperature();
}

void temperatureUp() {
  digitalWrite(THERMAL_CONTROL_DOWN, HIGH);
  digitalWrite(THERMAL_CONTROL_UP, LOW);
}
void temperatureDown() {
  digitalWrite(THERMAL_CONTROL_DOWN, LOW);
  digitalWrite(THERMAL_CONTROL_UP, HIGH);
}
void fixTemperature() {
  digitalWrite(THERMAL_CONTROL_DOWN, HIGH);
  digitalWrite(THERMAL_CONTROL_UP, HIGH);
}
boolean isThermalControlLow() {
  unsigned long thermalControlPulseLength = pulseIn(THERMAL_CONTROL_SCR, HIGH, THERMAL_CONTROL_SCR_TIMEOUT_MICROS);
  if (thermalControlPulseLength > 0) {
    return false;
  }
  return (digitalRead(THERMAL_CONTROL_SCR) == LOW);
}
boolean isThermalControlHigh() {
  unsigned long thermalControlPulseLength = pulseIn(THERMAL_CONTROL_SCR, HIGH, THERMAL_CONTROL_SCR_TIMEOUT_MICROS);
  if (thermalControlPulseLength > 0) {
    return false;
  }
  return (digitalRead(THERMAL_CONTROL_SCR) == HIGH);
}



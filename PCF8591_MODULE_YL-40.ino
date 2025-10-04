#include <Wire.h>

// Configurazione PCF8591
#define PCF8591_ADDR 0x48
#define ADC_RESOLUTION 255.0
#define NUM_CHANNELS 4

// Configurazione ESP8266 I2C
#define SDA_PIN 0  // GPIO0
#define SCL_PIN 2  // GPIO2

// Configurazione termistore (NTC 10kΩ)
const float VREF = 3.3;           // Tensione di riferimento
const float R_FIXED = 9000.0;     // Resistenza fissa 9kΩ
const float BETA = 3950.0;        // Coefficiente Beta del termistore
const float T0_KELVIN = 298.15;   // 25°C in Kelvin
const float R0 = 10000.0;         // Resistenza del termistore a 25°C

// Configurazione temporizzazione
const unsigned long SENSOR_DELAY = 10;    // Delay tra letture sensori (ms)
const unsigned long LOOP_DELAY = 1000;    // Delay principale del loop (ms)

// Struttura per i dati dei sensori
struct SensorData {
  int photoresistor;    // AIN0: Fotoresistore
  int moistureSensor;   // AIN1: Sensore umidità
  int thermistor;       // AIN2: Termistore (raw)
  float temperature;    // AIN2: Temperatura calcolata (°C)
  int potentiometer;    // AIN3: Potenziometro
  bool valid;           // Flag per indicare se i dati sono validi
};

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  
  Serial.println("\n=== Avvio Sistema Sensori PCF8591 ===");
  
  // Inizializzazione I2C con pin personalizzati per ESP8266
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  
  // Test connessione PCF8591
  if (!testPCF8591Connection()) {
    Serial.println("ERRORE: PCF8591 non rilevato! Controlli i collegamenti.");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("PCF8591 inizializzato correttamente");
  Serial.println("Inizio acquisizione dati...\n");
}

bool testPCF8591Connection() {
  Wire.beginTransmission(PCF8591_ADDR);
  return (Wire.endTransmission() == 0);
}

int readPCF8591(uint8_t channel) {
  if (channel >= NUM_CHANNELS) {
    Serial.print("ERRORE: Canale non valido: ");
    Serial.println(channel);
    return -1;
  }
  
  Wire.beginTransmission(PCF8591_ADDR);
  Wire.write(0x40 | channel);
  
  if (Wire.endTransmission() != 0) {
    Serial.print("ERRORE I2C su canale ");
    Serial.println(channel);
    return -1;
  }
  
  uint8_t bytesReceived = Wire.requestFrom(PCF8591_ADDR, 2);
  if (bytesReceived != 2) {
    Serial.print("ERRORE: Ricevuti solo ");
    Serial.print(bytesReceived);
    Serial.print(" byte invece di 2 per canale ");
    Serial.println(channel);
    return -1;
  }
  
  Wire.read(); // Prima lettura (valore precedente, da scartare)
  int value = Wire.read(); // Seconda lettura (valore corrente)
  
  return value;
}

float convertToCelsius(int adcValue) {
  if (adcValue < 0 || adcValue > 255) {
    return NAN;
  }
  
  float vOut = (adcValue * VREF) / ADC_RESOLUTION;
  
  if (vOut <= 0.01 || vOut >= (VREF - 0.01)) {
    return NAN;
  }
  
  float rNtc = (VREF * R_FIXED / vOut) - R_FIXED;
  
  if (rNtc <= 0 || isnan(rNtc)) {
    return NAN;
  }
  
  float tempKelvin = 1.0 / (1.0 / T0_KELVIN + (1.0 / BETA) * log(rNtc / R0));
  float tempCelsius = tempKelvin - 273.15;
  
  if (tempCelsius < -40.0 || tempCelsius > 85.0) {
    return NAN;
  }
  
  return tempCelsius;
}

SensorData readAllSensors() {
  SensorData data;
  bool allValid = true;
  
  data.photoresistor = readPCF8591(0);
  delay(SENSOR_DELAY);
  
  data.moistureSensor = readPCF8591(1);
  delay(SENSOR_DELAY);
  
  data.thermistor = readPCF8591(2);
  delay(SENSOR_DELAY);
  
  data.potentiometer = readPCF8591(3);
  
  if (data.photoresistor < 0) allValid = false;
  if (data.moistureSensor < 0) allValid = false;
  if (data.thermistor < 0) allValid = false;
  if (data.potentiometer < 0) allValid = false;
  
  if (data.thermistor >= 0) {
    data.temperature = convertToCelsius(data.thermistor);
    if (isnan(data.temperature)) {
      allValid = false;
    }
  } else {
    data.temperature = NAN;
  }
  
  data.valid = allValid;
  return data;
}

void printSensorData(const SensorData& data) {
  // [Fotoresistore AIN0] valore
  Serial.print("[Fotoresistore AIN0] ");
  if (data.photoresistor >= 0) {
    Serial.print(data.photoresistor);
  } else {
    Serial.print("ERRORE");
  }
  Serial.print("   ");
  
  // [Umidità AIN1] valore
  Serial.print("[Umidità AIN1] ");
  if (data.moistureSensor >= 0) {
    Serial.print(data.moistureSensor);
  } else {
    Serial.print("ERRORE");
  }
  Serial.print("   ");
  
  // [Termistore AIN2] valore (temperatura)
  Serial.print("[Termistore AIN2] ");
  if (data.thermistor >= 0) {
    Serial.print(data.thermistor);
    if (!isnan(data.temperature)) {
      Serial.print(" (");
      Serial.print(data.temperature, 2);
      Serial.print(" °C)");
    } else {
      Serial.print(" (ERRORE conversione)");
    }
  } else {
    Serial.print("ERRORE");
  }
  Serial.print("   ");
  
  // [Potenziometro AIN3] valore
  Serial.print("[Potenziometro AIN3] ");
  if (data.potentiometer >= 0) {
    Serial.print(data.potentiometer);
  } else {
    Serial.print("ERRORE");
  }
  
  // Fine riga
  Serial.println();
}

void loop() {
  SensorData sensorData = readAllSensors();
  printSensorData(sensorData);
  delay(LOOP_DELAY);
}

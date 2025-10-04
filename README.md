# PCF8591_MODULE_YL-40
# PCF8591 Sensor Reader per ESP8266 (esp-01s)


## Descrizione
Sistema di acquisizione dati da sensori analogici utilizzando modulo ADC PCF8591 con microcontrollore ESP-01s (ESP8266).
Questo progetto implementa un sistema di lettura multi-sensore tramite interfaccia I2C. Il codice gestisce quattro canali analogici del PCF8591 con conversione dati per sensori specifici.

### Sensori supportati

- **AIN0**: Fotoresistore (luminosità)
- **AIN1**: Sensore di umidità
- **AIN2**: Termistore NTC 10kΩ (con conversione a °C)
- **AIN3**: Potenziometro

## Caratteristiche

- Lettura sequenziale di 4 canali analogici
- Conversione temperatura basata su equazione Steinhart-Hart
- Gestione errori I2C con validazione dati
- Output formattato su seriale
- Configurazione parametrizzata per termistore NTC

## Hardware richiesto

- ESP-01s (ESP8266) o compatibile
- Modulo PCF8591 (ADC 8-bit con interfaccia I2C)
- Fotoresistore
- Sensore di umidità analogico
- Termistore NTC 10kΩ (Beta = 3950)
- Resistenza fissa 9kΩ per partitore tensione
- Potenziometro

## Schema collegamenti

### Pin I2C ESP-01s
- **SDA**: GPIO0
- **SCL**: GPIO2
- **VCC**: 3.3V
- **GND**: GND

### PCF8591
- **AIN0**: Fotoresistore
- **AIN1**: Sensore umidità
- **AIN2**: Termistore (con resistenza fissa 9kΩ)
- **AIN3**: Potenziometro
- **Indirizzo I2C**: 0x48 (default)

## Installazione

### Prerequisiti

- Arduino IDE 1.8.x o superiore / Arduino IDE 2.x
- ESP8266 Board Package installato
- Libreria Wire (inclusa)

### Caricamento sketch

1. Clonare il repository:
https://github.com/Diego-Fenu/PCF8591_MODULE_YL-40/edit/main/README.md

2. Aprire `pcf8591_sensor_reader.ino` con Arduino IDE

3. Selezionare la scheda: `Tools > Board > ESP8266 Boards > Generic ESP8266 Module`

4. Configurare i parametri:
   - Upload Speed: 115200
   - Flash Size: 1MB (secondo il modulo)

5. Caricare lo sketch

## Configurazione

### Parametri termistore

Se si utilizza un termistore diverso, modificare le costanti:
const float R_FIXED = 9000.0; // Resistenza fissa partitore (Ω)
const float BETA = 3950.0; // Coefficiente Beta termistore
const float R0 = 10000.0; // Resistenza a 25°C (Ω)


### Temporizzazione
const unsigned long SENSOR_DELAY = 10; // Delay tra letture canali (ms)
const unsigned long LOOP_DELAY = 1000; // Intervallo letture (ms)


## Utilizzo

1. Collegare il modulo PCF8591 e i sensori secondo lo schema
2. Alimentare l'ESP-01s a 3.3V
3. Aprire il Serial Monitor (115200 baud)
4. L'output mostrerà le letture formattate:
=== Avvio Sistema Sensori PCF8591 ===
PCF8591 inizializzato correttamente
Inizio acquisizione dati...

[Fotoresistore AIN0] 142 [Umidità AIN1] 98 [Termistore AIN2] 128 (24.35 °C) [Potenziometro AIN3] 200
[Fotoresistore AIN0] 145 [Umidità AIN1] 97 [Termistore AIN2] 129 (24.50 °C) [Potenziometro AIN3] 198



## Formato output
[Nome Sensore AINx] valore_grezzo (valore_calcolato)


- I valori grezzi sono nell'intervallo 0-255 (8-bit ADC)
- La temperatura è calcolata in gradi Celsius con 2 decimali
- Gli errori di lettura mostrano "ERRORE"

## Gestione errori

Il sistema rileva e segnala:
- Errori di comunicazione I2C
- Valori fuori range
- Errori di conversione temperatura
- Canali non validi


## Autore

Diego Fenu

## Contributi

I contributi sono benvenuti! Per favore:
1. Fai fork del progetto
2. Crea un branch per la feature (`git checkout -b feature/nuova-funzionalita`)
3. Commit delle modifiche (`git commit -m 'Aggiunge nuova funzionalità'`)
4. Push al branch (`git push origin feature/nuova-funzionalita`)
5. Apri una Pull Request

## Changelog

### v1.0.0 (2025-10-04)
- Release iniziale
- Supporto 4 canali PCF8591
- Conversione temperatura NTC
- Output formattato singola riga

## Riferimenti

- [Datasheet PCF8591](https://www.nxp.com/docs/en/data-sheet/PCF8591.pdf)
- [ESP8266 Arduino Core](https://github.com/esp8266/Arduino)
- [Wire Library Reference](https://www.arduino.cc/en/Reference/Wire)








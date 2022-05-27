#include <DHTesp.h>
#include <elapsedMillis.h>
#include "CDebounce.h"

DHTesp dht;
TempAndHumidity lastValues;
TempAndHumidity printedValues;

void DHT_setup(void){
    dht.setup(DHT_PIN, DHTesp::DHT11);
}

elapsedMillis DHT_loopTimer;
void DHT_loop(void){
    if (DHT_loopTimer < DHT_READ_INTERVAL_MS) return;
    DHT_loopTimer = 0;
    // Reading temperature and humidity takes about 250 milliseconds
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    lastValues = dht.getTempAndHumidity();

    if (lastValues.temperature != printedValues.temperature){   //only print changes
        Serial.println("Temperature: " + String(lastValues.temperature,1) + "°C");
        printedValues.temperature = lastValues.temperature;
    }
    if (lastValues.humidity != printedValues.humidity){
        Serial.println("Relative Humidity: " + String(lastValues.humidity,0) + "%");
        printedValues.humidity = lastValues.humidity;
    }
    
}

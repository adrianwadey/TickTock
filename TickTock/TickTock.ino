#include <DHTesp.h>
#include <elapsedMillis.h>
#include "CDebounce.h"

#define DEVICE_NAME "ticktock"  //name that this device will appear as on the local network

#define DHT_READ_INTERVAL_MS (5000)     //probably don't need to read this often, plus reading it warms it up!
                                        //so use a short time for testing it works and use a long time for accuracy

#define SW_PIN  (D3)            //NodeMCU has a switch on this pin already, use it for starting WPS connection
#define LED_PIN (D4)            //NodeMCU has an LED on this pin already
#define DHT_PIN (D1)            //Pin for communicating with the DHT11
Debounce WPSSwitch(SW_PIN);     //We would probably get away without debouncing this
bool WPSLedState = false;       //Indicator for WiFi connection status


void setup() {
    Serial.begin(115200);       //set the same rate in the terminal program
                                //74880 is the baud rate of the programmer and allows to see trailing messages
    Serial.setTimeout(1000000); //for debug stepping with Serial.readStringUntil('\r'); Ensure terminal sends CR
    Serial.println("      Internet Clock with      ");
    Serial.println("Humidity and temperature sensor");
    Serial.println("===============================");

    //Initialise the LCD
    LCD_setup();    //this also shows the startup message, so waits until it's done before starting everything else
    
    pinMode(SW_PIN, INPUT_PULLUP);      //WPS switch input
    
    //Initialise WiFi
    WiFi_setup();
    // Initialize temperature sensor
    DHT_setup();
    //Configure the web server
    WebServer_setup();
    //set up NTP
    NTP_setup();
}

void loop() {
    DHT_loop();
    WiFi_loop();
    WebServer_loop();
    NTP_loop();
    LCD_loop();
}

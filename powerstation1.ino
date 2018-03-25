#define BLYNK_PRINT Serial
#define PIXEL_COUNT 150
#define PIXEL_PORT D2
#define PIXEL_PORT2 D4
#define PIXEL_TYPE WS2812B
#define BLYNK_RED "#FF0000"
#define BLYNK_WHITE "#FFFFFF"
#define BLYNK_BLUE "#0000FF"

#include "MQTT.h"
#include "blynk.h"
#include "lightshow.h"
#include "transitions.h"
#include "color-pickers.h"
#include "color-filters.h"

// create the lightshow
LightShow lightshow(PIXEL_COUNT, PIXEL_PORT, PIXEL_TYPE);
LightShow lightshow2(PIXEL_COUNT, PIXEL_PORT2, PIXEL_TYPE);
BrightnessColorFilter brightness;
NoiseColorFilter noise;
ImmediateTransition immediate;
WheelTransition wheel(255);
// Sets one pixels color per cycle
WipeTransition wipe;
SolidColorPicker solid;

// Initialize MQTT Client and parse data
void callback(char* topic, byte* payload, unsigned int length);
byte server[] = { 192, 168, 1, 75 }; //IP Address for Raspberry Pi MQTT Server
MQTT client(server, 1883, callback);

bool blynk_ready = false;

char auth[] = "YourBLYNKAuthToken";

void setup() {
    Serial.begin(9600);
    delay(5000);
    Blynk.begin(auth);
    
    //Connect to Raspberry Pi Broker
    client.connect("PowerStation");
    
    if (client.isConnected()) {
        client.subscribe("rfid/access/");
        client.subscribe("led/color/");
    }
    
    lightshow.setup();
    lightshow.speed(25);
    lightshow.addColorFilter(&brightness);
    // set the initial transition driver
    lightshow.useTransition(&wipe);
    lightshow.repeat(false);
    WhiteLights();
    
    lightshow2.setup();
    lightshow2.speed(25);
    lightshow2.addColorFilter(&brightness);
    // set the initial transition driver
    lightshow2.useTransition(&wipe);
    lightshow2.repeat(false);
    WhiteLights2();
    
}
// recieve mqtt message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    if (!strcmp(p, "denied")) {
        //Green();
        
    }else if (!strcmp(p, "granted")) {
        RedLights();
        Particle.publish("PowerStation Wheelwell", "Lights Red");
        
        
    }else if (!strcmp(p, "exit")) {
        WhiteLights();
       
    }
    //delay(1000);
   
}


void WhiteLights() {
    lightshow.repeat(false);
    lightshow.useTransition(&immediate);
    lightshow.useColorPicker(&solid);
    solid.setColor(255,255,255);
    Particle.publish("PowerStaton Wheelwell", "Lights White");
}   
void WhiteLights2() { 
    lightshow2.repeat(false);
    lightshow2.useTransition(&immediate);
    lightshow2.useColorPicker(&solid);
    solid.setColor(255,255,255);
    
}
void RedLights() {
    lightshow.repeat(false);
    lightshow.useTransition(&immediate);
    lightshow.useColorPicker(&solid);
    solid.setColor(255,0,0);
   
}

void loop() {
    //MQTT Connection
    if (client.isConnected()){
        client.loop();
    }
    Blynk.run();
    
    //Run Light Shows
    lightshow.update();
    lightshow2.update();
    
}

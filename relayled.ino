#define BLYNK_PRINT Serial
#define PIXEL_COUNT 150
#define PIXEL_PORT D2
#define PIXEL_TYPE WS2812B
#define BLYNK_RED "#FF0000"
#define BLYNK_WHITE "#FFFFFF"
#define BLYNK_BLUE "#0000FF"

#include <MQTT.h>
#include <blynk.h>
//#include <NCD4Relay.h>
#include <lightshow.h>
#include <transitions.h>
#include <color-pickers.h>
#include <color-filters.h>


//NCD4Relay controller;
// create the lightshow
LightShow lightshow(PIXEL_COUNT, PIXEL_PORT, PIXEL_TYPE);
BrightnessColorFilter brightness;
ImmediateTransition immediate;
WheelTransition wheel(255);
// Sets one pixels color per cycle
WipeTransition wipe;
// causes all of the pixels to be set to the same color
SolidColorPicker solid;

void callback(char* topic, byte* payload, unsigned int length);
byte server[] = { 192, 168, 1, 50 };
MQTT client(server, 1883, callback);

bool blynk_ready = false;
int callcount=0;

char auth[] = "e62a3458b30848e4b3ce94dfb0f43918";

void setup() {
    Serial.begin(9600);
    delay(2000000);
    Blynk.begin(auth);
    //Connect to Raspberry Pi Broker
    client.connect("RelayLights");
    
    if (client.isConnected()) {
        client.subscribe("rfid/access/");
        Particle.publish("Client Subscribed");
    }
   
    lightshow.setup();
    lightshow.speed(25);
    lightshow.addColorFilter(&brightness);
    // set the initial transition driver
    lightshow.useTransition(&wipe);
    lightshow.repeat(false);
    
    WhiteLights();
    
}
// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    if (!strcmp(p, "denied")) {
        Green();
        
    }else if (!strcmp(p, "granted")) {
        RedLights();
        //Particle.publish("Relay", "Relay Opening Door");
        Particle.publish("Relay Wheelwell", "Lights Red");
        
    }else if (!strcmp(p, "exit")) {
        WhiteLights();
        
    }
   
}

void WhiteLights() {
    lightshow.repeat(false);
    lightshow.useTransition(&immediate);
    lightshow.useColorPicker(&solid);
    solid.setColor(255,255,255);
    
}
void RedLights() {
    lightshow.repeat(false);
    lightshow.useTransition(&immediate);
    lightshow.useColorPicker(&solid);
    solid.setColor(255,0,0);
 
    //delay(4000);
    //WhiteLights();
   
}

void loop() {
    if (client.isConnected()){
        client.loop();
    }
    Blynk.run();
    lightshow.update();
  
}

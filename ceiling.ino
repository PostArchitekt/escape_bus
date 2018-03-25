#define BLYNK_PRINT Serial
#define PIXEL_COUNT 200
#define PIXEL_PORT D2
#define PIXEL_TYPE WS2812B
#define BLYNK_RED "#FF0000"
#define BLYNK_WHITE "#FFFFFF"
#define BLYNK_BLUE "#0000FF"

#include <MQTT.h>
#include <blynk.h>
//#include <stdlib.h>
#include <lightshow.h>
#include <transitions.h>
#include <color-pickers.h>
#include <color-filters.h>

BlynkTimer timer;

// create the lightshow
LightShow lightshow(PIXEL_COUNT, PIXEL_PORT, PIXEL_TYPE);
BrightnessColorFilter brightness;
NoiseColorFilter noise;
ImmediateTransition immediate;
WheelTransition wheel(255);
WipeTransition wipe;
SolidColorPicker solid;
FadeColorPicker fade;

WidgetLCD lcd(V21);

int mission1, mission2;
bool stationlights = true;

// Initialize MQTT Client and parse incoming data
void callback(char* topic, byte* payload, unsigned int length);
byte server[] = { 192, 168, 1, 75 }; //IP Address of Raspberry Pi MQTT Broker
MQTT client(server, 1883, callback);

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    if (!strcmp(p, "denied")) {
        //RedFlash();
        
    }else if (!strcmp(p, "granted")) {
       // RedLights();
        
    }else if (!strcmp(p, "startTimer")) {
        mission1 = 0;
    }else if (!strcmp(p, "exit")) {
        //WhiteLights();
        //Mode(1);
    }
    delay(1000);
   
}

bool blynk_ready = false;

char auth[] = "YourBLYNKAuthToken";

int ledWarning1;
int ledWarning2;
int digiclock, cntdwn, cntdwnMin, cntdwnSec ;
int mode;

void setup() {
    //Serial.begin(9600);
    delay(5000);
    Blynk.begin(auth);
    // Start Mqtt connction
    client.connect("Ceiling");
 
      if (client.isConnected()) {
          client.subscribe("rfid/access/");
          client.subscribe("led/color/");
          //client.subscribe("video/play");
      }

    ledWarning1 = timer.setInterval(1000L, missionBegin);
    ledWarning2 = timer.setInterval(1000L, missionEnd);
    digiclock = timer.setInterval(1000L, DigitalClockDisplay);
    
    // setup the lightshow
    lightshow.setup();
    lightshow.speed(25);
    lightshow.addColorFilter(&brightness);
    lightshow.useTransition(&wipe);
    lightshow.repeat(true);

    Blynk.virtualWrite(V29, 0);
}

// Start mission timer for controlling overhead lightshows
void missionBegin() {
    if (mission1 < 4){
         Mode(1);
         
    }else if (mission1 > 70 && mission1 < 130){
        RedLights();
        
        if (mission1 > 70 && mission1 < 82) {
        Particle.publish("PowerStation", "Powering Down");
        client.publish("led/color/", "powerblack");
        }
        
    }else if (mission1 > 131 && mission1 < 1691){
        Mode(1);
        
        //5 Second Red
    }else if (mission1 > 1692 && mission1 < 1697){
        RedLights();
        
    //Switch Back to normal
    }else if (mission1 > 1698 && mission1 < 1703){
        Mode(1);
    
    //Lights Flash Red for 30 secs
    }else if (mission1 > 1704 && mission1 < 1734){
        Mode(3);
        
    //Lights Solid Red
    }else if (mission1 > 1735 && mission1 < 1941){
        RedLights();
        
    //Lights Normal White
    } else { (mission1 > 1941); 
        Mode(1);
    }
    mission1++;
    
}
void missionEnd() {
   
    if (mission2 <= 1942){
        
        cntdwnMin = (1942 - mission2)/60;
       
        lcd.print(3, 0, cntdwnMin);
        lcd.print(6, 0, "Minutes");
        //lcd.print(9, 0, cntdwnSec);
        lcd.print(6, 1, "Left");

    }else{
    
    }
     mission2++;
    
}

void DigitalClockDisplay() {
  int m,s;
  s = mission1;
  m = s / 60;
  s = s - m * 60;
  
}

void WhiteFlash() {
    lightshow.repeat(false);
    lightshow.useTransition(&immediate);
    lightshow.useColorPicker(&solid);
    solid.setColor(255,255,255);

    timer.setTimer(1000L, RedLights, 10);

}
void RedFlash() {
    lightshow.repeat(false);
    lightshow.useTransition(&immediate);
    lightshow.useColorPicker(&solid);
    solid.setColor(255,0,0);
    timer.setTimer(1000L, WhiteFlash, 10);
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
    
}

void Blue() {
    lightshow.repeat(false);
    lightshow.useTransition(&wipe);
    lightshow.useColorPicker(&solid);
    solid.setColor(140, 0, 255);

    //timer.setTimer(1000L, WhiteFlash, 10);

}
void Mode(int mode) { 
    if(mode == 1) {  
        lightshow.repeat(false);
        lightshow.useTransition(&wipe);
        lightshow.useColorPicker(&solid);
        solid.setColor(255, 255, 255);
      
    } else if (mode == 3) {
       timer.setTimer(500L, WhiteFlash, 10);
       Blynk.setProperty(V23, "color", BLYNK_RED);
        
        
    } else if (mode == 4) {
            }
    
    Blynk.virtualWrite(V20, mode);
}
/*
if (client.isConnected()) {
        client.subscribe("relay/");
        client.subscribe("rfid/access/");
        client.publish("led/color/", );
        client.publish("rfid/access", )

    }*/
BLYNK_WRITE(V8){
    mission1=0;
    mission2=0;
  
  Blynk.virtualWrite(V19, mission1);
 
}

BLYNK_READ(V19){
    Blynk.virtualWrite(V19, mission1);
}

BLYNK_READ(V20){
    Blynk.virtualWrite(V20, mode);
}
BLYNK_READ(V21){
    Blynk.virtualWrite(V23, mode);
}

void loop() {
    if (client.isConnected()) {
        client.loop();
    }
    
    Blynk.run();
    
    lightshow.update();
    timer.run();
}

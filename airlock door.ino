#include <clickButton.h>
#include <MQTT.h>
#include <blynk.h>
#include <lightshow.h>
#include <transitions.h>
#include <color-pickers.h>
#include <color-filters.h>

#define PIXEL_COUNT 60
#define PIXEL_PORT D2
#define PIXEL_TYPE WS2812B
#define BLYNK_RED "#FF0000"
#define BLYNK_WHITE "#FFFFFF"
#define BLYNK_GREEN "#008000"
#define BLYNK_BLUE "#0000FF"

int count = 0;                                          // count = 0
char input[12];                                         // character array of size 12 
boolean flag = 0;
int function = 0; 

const int buttonPin1 = A2;
ClickButton button1(buttonPin1, HIGH, CLICKBTN_PULLUP);

// Initialize MQTT Client
void callback(char* topic, byte* payload, unsigned int length);
byte server[] = { 192, 168, 1, 75 };                   //IP Address of Raspberry Pi Broker
MQTT client(server, 1883, callback);

char auth[] = "YourBLYNKAuthToken";


// recieve mqtt message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

}

void setup()
{
    client.connect("AirlockDoor");
    pinMode(D5, INPUT_PULLUP);

   // Setup button timers (all in milliseconds / ms)
    button1.debounceTime   = 20;                      // Debounce timer in ms
    //button1.multiclickTime = 250;                   // Time limit for multi clicks
    //button1.longClickTime  = 1000;
    
    Serial1.begin(9600);                              // begin serial port with baud rate 9600bps
    
    Blynk.begin(auth);
    Blynk.setProperty(V9, "color", BLYNK_WHITE);

}

void AirlockControl(){
    
   if(Serial1.available())
   {
      count = 0;
      while(Serial1.available() && count < 12)         // Read 12 characters and store them in input array
      {
         input[count] = Serial1.read();
         count++;
         delay(5);
      }
     
    Particle.publish("RFID Access Request ", input);
     
    if (client.isConnected()) {
        client.publish("rfid/access/","rfid request");
        }
        
    Blynk.setProperty(V9, "color", BLYNK_BLUE);     
        Particle.publish("RFID Access Granted ", input);
        client.publish("rfid/access/","granted");
              
    Blynk.setProperty(V9, "color", BLYNK_GREEN);
    
     /*
      Serial1.print(input);                             // Print RFID tag number 
      Serial.println("RFID Access Request: ");
      Serial.println(input);
    */ 
      
      if ( strcmp(input,"your_rfid_tagnumber") == 0 ) {
          
        Particle.publish("RFID Access Granted ", input);
        client.publish("rfid/access/","granted");
              
        Blynk.setProperty(V9, "color", BLYNK_GREEN);
          
      }else{
         Particle.publish("RFID Access Denied ", input);
         client.publish("rfid/access/","denied");
              
         Blynk.setProperty(V9, "color", BLYNK_RED);
              
         delay(2000);
         Blynk.setProperty(V9, "color", BLYNK_WHITE);    
      }
     
   }
   
       
    if(button1.clicks != 0) function = button1.clicks;
  
    if(function == 1) {
       Particle.publish("Inside Airlock Switch ", "Exit Granted");
       client.publish("rfid/access/","granted");
   
      function = 0;
      delay(5);
   }
   
   
   
}

BLYNK_WRITE(V30) {
    if (param.asInt() == 1){
        Blynk.setProperty(V30, "color", BLYNK_RED);
        Blynk.virtualWrite(V11, 1);
        
    }else if (param.asInt() == 2){
        Blynk.setProperty(V30, "color", BLYNK_BLUE);
        Blynk.virtualWrite(V11, 2);
        
    }else {
        Blynk.setProperty(V30, "color", BLYNK_WHITE);
        Blynk.virtualWrite(V11, 3);
        Particle.publish("App Access", "Granted" );
        //client.publish("rfid/access/","granted");
        
     }
 
    }

void loop() { 

    if (client.isConnected()) {
    client.loop();
  }
    button1.Update();
    
    AirlockControl();
    Blynk.run();
}



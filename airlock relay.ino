#define BLYNK_PRINT Serial
#include <MQTT.h>
#include <blynk.h>
#include <NCD4Relay.h>

NCD4Relay controller;

// Initialize MQTT Client and parse data
void callback(char* topic, byte* payload, unsigned int length);
byte server[] = { 192, 168, 1, 50 }; // Initialize MQTT Client and parse data
MQTT client(server, 1883, callback);

bool blynk_ready = false;
int callcount=0;

char auth[] = "YourBLYNKAuthToken";

void setup() {
    Serial.begin(9600);
    delay(5000);
    Blynk.begin(auth);
    
    //Connect to Raspberry Pi Broker
    client.connect("AirlockRelay");
    if (client.isConnected()) {
        client.subscribe("rfid/access/");
        
    }
    controller.setAddress(0,0,0);
    controller.turnOffAllRelays();
}
// recieve mqtt message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    if (!strcmp(p, "denied")) {
        
    }else if (!strcmp(p, "granted")) {
        Particle.publish("Relay", "Relay Opening Door");
        Particle.publish("Relay Wheelwell", "Lights Red");
        airlock(1);
        
    }else if (!strcmp(p, "exit")) {
        
        
    }
    
}

void airlock(int relay) {
  controller.turnOnRelay(relay);
  Blynk.virtualWrite(V26, 1);
  delay(4000);

  controller.turnOffRelay(relay);
  Blynk.virtualWrite(V26, 0);
  Particle.publish("Relay", "Relay Closing Door");
  client.publish("rfid/access/", "exit");

}

void loop() {
    if (client.isConnected()){
        client.loop();
    }
    Blynk.run();
    
  
}

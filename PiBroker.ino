#define BLYNK_PRINT Serial

#include <blynk.h>
#include <stdlib.h>
#include <MQTT.h>

char auth[] = "YourBLYNKAuthToken";

BLYNK_WRITE(V12)
  {
  String action = param.asStr();
  
  // Start Video Playing
  if (action == "play") {
    Particle.publish("Video", "now playing");
    system("omxplayer --no-keys -b -o hdmi /home/pi/Videos/Path_To_File.mp4 &");
    
  }
  // Kill Video Playing
  if (action == "stop") {
    
    system("sudo kill -9 $(pgrep omxplayer)");
    Particle.publish("Video", "now stopped");
   
  }
  
  Blynk.setProperty(V12, "label", action);
  Serial.print(action);
  Serial.println();
  }

void setup()
  {
  // Debug console
  Serial.begin(9600);
  
  // Allow board to settle
  delay(5000); 
  Blynk.begin(auth);
  }

void loop()
  {  
  Blynk.run();
  }

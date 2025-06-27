

/*
Connection Tutorial:

1. Connect the LD6002 radar sensor to your ESP32 board as follows:
  - LD6002 TX  -> ESP32 GPIO16 (RX2)
  - LD6002 RX  -> ESP32 GPIO17 (TX2)
  - LD6002 GND -> ESP32 GND
  - LD6002 VCC -> ESP32 3.3V or 5V (check your module's voltage requirements)

2. Make sure you have installed the "LD6002.h" library in your Arduino IDE.

3. Upload this sketch to your ESP32 board.

4. Open the Serial Monitor at 115200 baud to view heart rate, breath rate, and distance data.

*/
#include <Arduino.h>
#include "LD6002.h"
LD6002 radar(Serial1);

void setup()
{
  Serial.begin(115200);
  Serial1.begin(1382400, SERIAL_8N1, 16, 17);

}

float lastHeartRate = 0;
float lastBreathRate = 0;
float lastDistance = 0;
void loop()
{
  radar.update();

  if (radar.hasNewHeartRate())
  {
   float heartRateMain = radar.getHeartRate();
   if ((heartRateMain != lastHeartRate) && (heartRateMain > 0))
   {
    Serial.printf("Heart Rate: %.2f bpm\n", heartRateMain);
   }
   lastHeartRate = heartRateMain;
   radar.clearHeartRateFlag();
  }

  if (radar.hasNewBreathRate())
  {
   float breathRateMain = radar.getBreathRate();
   if ((breathRateMain != lastBreathRate) && (breathRateMain > 0))
   {
    Serial.printf("Breath Rate: %.2f bpm\n", breathRateMain);
   }
   lastBreathRate = breathRateMain;
   radar.clearBreathRateFlag();
  }

  if (radar.hasNewDistance())
  {
   float distanceMain = radar.getDistance();
   if ((distanceMain != lastDistance) && (distanceMain > 0))
   {
    Serial.printf("Distance: %.2f cm\n", distanceMain);
   }
   lastDistance = distanceMain;
   radar.clearDistanceFlag();
  }
}
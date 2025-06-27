

// RadarParser.h
#pragma once
#include <Arduino.h>
class LD6002
{
public:
  enum class FrameType : uint16_t
  {
    HeartRate = 0x0A15,
    BreathRate = 0x0A14,
    Distance = 0x0A16,
    Unknown = 0xFFFF
  };

  LD6002(HardwareSerial &serial);
  void update();

  bool hasNewHeartRate() const;
  bool hasNewBreathRate() const;
  bool hasNewDistance() const;

  float getHeartRate() const;
  float getBreathRate() const;
  float getDistance() const;

  void clearHeartRateFlag();
  void clearBreathRateFlag();
  void clearDistanceFlag();

private:
  static constexpr uint8_t MAX_FRAME_LEN = 17;
  HardwareSerial &serial;
  uint8_t frame[MAX_FRAME_LEN];
  uint8_t pos = 0;
  bool syncing = false;
  uint8_t expectedFrameLen = 0;

  float heartRate = 0;
  float breathRate = 0;
  float distance = 0;
  bool newHeartRate = false;
  bool newBreathRate = false;
  bool newDistance = false;

  float bytesToFloat(const uint8_t *data) const;
  uint32_t bytesToUInt32(const uint8_t *data) const;
  uint8_t calcXorInverse(const uint8_t *data, int len) const;
  FrameType getFrameType(const uint8_t *frame) const;
  void parseFrame(const uint8_t *frame);
  void printHex(const uint8_t *data, int len) const;
};

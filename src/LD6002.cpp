

// LD6002.cpp
#include "LD6002.h"
LD6002::LD6002(HardwareSerial &serial) : serial(serial) {}
void LD6002::update() {
  while (serial.available()) {
    uint8_t byte = serial.read();
    if (!syncing) {
      if (byte == 0x01) {
        frame[0] = byte;
        pos = 1;
        syncing = true;
        expectedFrameLen = 0;
      }
    } else {
      if (pos < MAX_FRAME_LEN) {
        frame[pos++] = byte;

        if (pos == 7) {
          uint16_t dataLen = (frame[3] << 8) | frame[4];
          expectedFrameLen = 8 + dataLen + 1;
          if (expectedFrameLen > MAX_FRAME_LEN) {
            syncing = false;
            pos = 0;
          }
        }

        if (expectedFrameLen > 0 && pos >= expectedFrameLen) {
          parseFrame(frame);
          syncing = false;
          pos = 0;
          expectedFrameLen = 0;
        }
      } else {
        syncing = false;
        pos = 0;
        expectedFrameLen = 0;
      }
    }
  }
}

bool LD6002::hasNewHeartRate() const { return newHeartRate; }
bool LD6002::hasNewBreathRate() const { return newBreathRate; }
bool LD6002::hasNewDistance() const { return newDistance; }

float LD6002::getHeartRate() const {
  return heartRate;
}

float LD6002::getBreathRate() const {
  return breathRate;
}

float LD6002::getDistance() const {
  return distance;
}

void LD6002::clearHeartRateFlag() {
  newHeartRate = false;
}

void LD6002::clearBreathRateFlag() {
  newBreathRate = false;
}

void LD6002::clearDistanceFlag() {
  newDistance = false;
}

float LD6002::bytesToFloat(const uint8_t *data) const {
  float f;
  memcpy(&f, data, sizeof(float));
  return f;
}

uint32_t LD6002::bytesToUInt32(const uint8_t *data) const {
  uint32_t val;
  memcpy(&val, data, sizeof(uint32_t));
  return val;
}

uint8_t LD6002::calcXorInverse(const uint8_t *data, int len) const {
  uint8_t cksum = 0;
  for (int i = 0; i < len; i++) cksum ^= data[i];
  return ~cksum;
}

LD6002::FrameType LD6002::getFrameType(const uint8_t *frame) const {
  uint16_t type = (frame[5] << 8) | frame[6];
  switch (type) {
    case 0x0A15: return FrameType::HeartRate;
    case 0x0A14: return FrameType::BreathRate;
    case 0x0A16: return FrameType::Distance;
    default:     return FrameType::Unknown;
  }
}

void LD6002::parseFrame(const uint8_t *frame) {
  if (calcXorInverse(frame, 7) != frame[7]) return;
  FrameType type = getFrameType(frame);

  switch (type) {
    case FrameType::HeartRate:
    {
      if (calcXorInverse(frame + 8, 4) != frame[12]) return;
      heartRate = bytesToFloat(&frame[8]);
      newHeartRate = true;
      break;
    }
    case FrameType::BreathRate:
    {
      if (calcXorInverse(frame + 8, 4) != frame[12]) return;
      breathRate = bytesToFloat(&frame[8]);
      newBreathRate = true;
      break;
    }
    case FrameType::Distance:
     { if (calcXorInverse(frame + 8, 8) != frame[16]) return;
      uint32_t flag = bytesToUInt32(&frame[8]);
      if (flag == 1) {
        distance = bytesToFloat(&frame[12]);
        newDistance = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  // Serial.print("DATA HEX: ");
  // printHex(frame + 8, (type == FrameType::Distance) ? 8 : 4);
  // Serial.println();
}

void LD6002::printHex(const uint8_t *data, int len) const {
  for (int i = 0; i < len; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
}

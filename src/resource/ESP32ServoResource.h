/*
 * This file is part of bowler-rpc-arduino.
 *
 * bowler-rpc-arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bowler-rpc-arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with bowler-rpc-arduino.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef ESP32SERVORESOURCE_H
#define ESP32SERVORESOURCE_H

#if defined(PLATFORM_ESP32)
#include "../commands/DiscoveryMetadata.h"
#include "Resource.h"
#include <Arduino.h>
#include <ESP32Servo.h>
#include <cstring>

class ServoResource : public Resource {
  public:
  ServoResource(std::uint8_t resource, std::uint8_t attachment, const std::uint8_t *attachmentData)
    : Resource(resource, attachment, attachmentData), pin(attachmentData[0]) {
    if (attachment == ATTACHMENT_POINT_TYPE_PWM_PIN) {
      std::uint8_t pin = attachmentData[0];
      std::uint16_t minUsLow = (attachmentData[1] << 8) | attachmentData[2];
      std::uint16_t minUsHigh = (attachmentData[3] << 8) | attachmentData[4];
      std::uint8_t timerWidth = attachmentData[5];
      servo.setTimerWidth(timerWidth);
      servo.attach(pin, minUsLow, minUsHigh);
    } else {
      servo.attach(pin);
    }
  }

  virtual ~ServoResource() {
    Serial.printf("Servo detach pin %d\n", pin);
    servo.detach();
  }

  void readFromPayload(std::uint8_t *buffer) override {
    servo.write(buffer[0]);
  }

  void writeToPayload(std::uint8_t *buffer) override {
  }

  protected:
  std::uint8_t pin;
  Servo servo;
};

/**
 * Checks if the attachment data is valid for the resource implementation.
 *
 * @param attachmentData The attachment data to validate.
 * @return The status code; `STATUS_ACCEPTED` if the attachment data is valid.
 */
static std::uint8_t validateServoAttachmentData(const std::uint8_t *attachmentData) {
  std::uint8_t pin = attachmentData[0];
  std::uint16_t minUsLow = (attachmentData[1] << 8) | attachmentData[2];
  std::uint16_t minUsHigh = (attachmentData[3] << 8) | attachmentData[4];
  std::uint8_t timerWidth = attachmentData[5];

  if (pin == 4 || pin == 5 || (pin >= 12 && pin <= 19) || (pin >= 21 && pin <= 23) ||
      (pin >= 25 && pin <= 27) || pin == 32 || pin == 33) {
    return STATUS_ACCEPTED;
  } else if (minUsLow < MIN_PULSE_WIDTH || minUsLow > MAX_PULSE_WIDTH) {
    return STATUS_REJECTED_INVALID_ATTACHMENT_DATA;
  } else if (minUsHigh < MIN_PULSE_WIDTH || minUsHigh > MAX_PULSE_WIDTH) {
    return STATUS_REJECTED_INVALID_ATTACHMENT_DATA;
  } else if (timerWidth < 16 || timerWidth > 20) {
    return STATUS_REJECTED_INVALID_ATTACHMENT_DATA;
  } else {
    return STATUS_REJECTED_INVALID_ATTACHMENT_DATA;
  }
}

#endif
#endif

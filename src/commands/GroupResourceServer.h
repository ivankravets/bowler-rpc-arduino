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
#pragma once

#include "../resource/Resource.h"
#include <Esp32SimplePacketComs.h>
#include <SimplePacketComs.h>
#include <cstring>
#include <vector>

class GroupResourceServer : public PacketEventAbstract {
  public:
  GroupResourceServer(std::uint8_t ipacketId) : PacketEventAbstract(ipacketId) {
  }

  virtual ~GroupResourceServer() {
  }

  void event(float *buffer) override {
    std::uint8_t *buf = (std::uint8_t *)buffer;

    for (auto &resource : resources) {
      resource->readFromPayload(buf);
      buf = buf + resource->getReceivePayloadLength();
    }

    buf = (std::uint8_t *)buffer;
    std::memset(buf, 0, PAYLOAD_LENGTH * (sizeof buf[0]));

    for (auto &resource : resources) {
      resource->writeToPayload(buf);
      buf = buf + resource->getSendPayloadLength();
    }
  }

  void addResource(std::unique_ptr<Resource> resource) {
    resources.push_back(std::move(resource));
  }

  protected:
  std::vector<std::unique_ptr<Resource>> resources{};
};
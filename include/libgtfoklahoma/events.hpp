/*
 * This file is part of the libgtfoklahoma distribution (https://github.com/arenson/libgtfoklahoma)
 * Copyright (c) 2020 Josh Arenson.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <libgtfoklahoma/event_model.hpp>

namespace libgtfoklahoma {

class Game;
class IEventObserver;
class Events {
public:
  explicit Events(Game &game, const char *eventJson=kEventJson);

  [[nodiscard]] EventModel &getEvent(int32_t id);
  void handleEvent(int32_t id, const std::unique_ptr<IEventObserver> &observer);

  std::vector<int32_t> eventsAtMile(int32_t mile);
  [[nodiscard]] bool hasMoreEvents(int32_t mile) const;

public:
  inline static EventModel kEmptyEventModel;

private:
  Game &m_game;
  std::unordered_map<int32_t, EventModel> m_eventsById;
  std::map<int32_t, std::vector<int32_t>> m_eventsByMile;
  inline static const char *kEventJson = R"JSON(
  [
    {
      "id": 0,
      "actions": [0],
      "description": "You awaken in a bed at the Frampton Inn. You recall hearing that California King Size beds are sold in states with tall and narrow residents and traditional King Size beds are for those states with a primarily short an wide populous. Your regular King Size bed stinks of cigarettes and if the popcorn ceiling could talk it would probably plead for euthanization. You get out of bed and stumble over to the window. As you feel the crunchiness of the motel carpet beneath your feet you deem it prudent to slip into your shower shoes. A glance out the window reveals a flat wasteland. A pickup truck with a bumper sticker reading 'Boomer Sooner' confirms your suspicion that you are, indeed, in the state of Oklahoma. A peek at your phone pinpoints you at Miami, OK, a town known for its rich lead deposits. All hope is not lost as you see your bike and full touring kit on the other side of the room! While you recognize that you are fairly close to Kansas, Misouri, and Arkansas, you know your only change for deliverance is to get the fuck out of Oklahoma and escape to the great state of Texas.",
      "display_name": "Frampton Inn - Miami, OK",
      "mile": 0
    },
    {
      "id": 1,
      "actions": [0, 10],
      "description": "Headquartered in Oklahoma City, this sprawling paradise is an American sized convenience store that also dabbles in the sale of CB radios.",
      "display_name": "Like's Travel Stop And Country Store",
      "mile": 2
    }
  ]
  )JSON";
};
}

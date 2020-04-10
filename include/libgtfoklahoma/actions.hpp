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

#include <map>
#include <memory>
#include <unordered_set>

#include <libgtfoklahoma/action_model.hpp>
#include <libgtfoklahoma/stats.hpp>

namespace libgtfoklahoma {

class Game;
class IEventObserver;
class Items;
class Actions : public std::enable_shared_from_this<Actions> {
public:
  explicit Actions(Game &game, const char *actionJson=kActionJson);

  [[nodiscard]] ActionModel &getAction(int32_t id);
  void handleAction(int32_t id, const std::unique_ptr<IEventObserver> &observer);

  // For things that are dependent on actions having occurred
  bool actionHasHappened(int32_t actionId);
  void setActionsThatHaveAlreadyHappened(std::unordered_set<int32_t> actionIds);

public:
  ActionModel kEmptyActionModel;

private:
  Game &m_game;
  std::map<int32_t, ActionModel> m_actions;
  std::unordered_set<int32_t> m_actionsThatHaveAlreadyHappened;

  inline static const char *kActionJson = R"JSON(
  [
    {
      "comment": "Default skip event",
      "display_name": "GTFO",
      "id": 0,
      "type": ["NONE"]
    },
    {
      "comment": "Custom skip event",
      "display_name": "Crap.",
      "id": 1,
      "type": ["NONE"]
    },
    {
      "comment": "Initial Store",
      "display_name": "Enter store",
      "id": 10,
      "items": [0],
      "type": ["STORE"]
    },
    {
      "comment": "This ironically makes your faster as it increases comfort",
      "id": 20,
      "display_name": "Spark a doobie",
      "type": ["STAT_CHANGE"],
      "stat_changes": [
        {"max_mph": 1}
      ]
    }
  ]
  )JSON";
};
}

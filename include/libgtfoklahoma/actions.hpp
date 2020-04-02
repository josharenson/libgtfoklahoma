/*
 * This file is part of the libgtfoklahoma distribution (https://github.com/arenson/gtfoklahoma)
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

#include <future>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include <rapidjson/document.h>

#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/stats.hpp>

namespace libgtfoklahoma {
struct ActionModel {
  // TODO: Can probably get some compile time type checking by doing something polymorphic here

  // Actions can be more than one type at a time so use a mask
  enum ActionType {
    NONE = 0,
    STAT_CHANGE = 1u << 0u,
    STORE = 1u << 1u
  };

  std::string display_name;
  int32_t id {-1};

  // If action is STORE type
  std::vector<int32_t> item_ids;
  void completePurchase() {
    m_purchasedItems.set_value(m_itemsPendingPurchase);
  }
  void purchaseItem(int32_t id_to_buy) {
    m_itemsPendingPurchase.push_back(id_to_buy);
  }
  std::future<std::vector<int32_t>> purchasedItems() {
    return m_purchasedItems.get_future();
  }

  // If action is STAT_CHANGE type
  StatModel stat_delta;

  uint32_t type;
  [[nodiscard]] bool isNoneType() const { return type & ActionType::NONE; }
  [[nodiscard]] bool isStatChangeType() const { return type & ActionType::STAT_CHANGE; }
  [[nodiscard]] bool isStoreType() const { return type & ActionType::STORE; }

private:
  std::vector<int32_t> m_itemsPendingPurchase;
  std::promise<std::vector<int32_t>> m_purchasedItems;
};

class IEventObserver;
class Items;
class Actions {
public:
   explicit Actions(Game &game, const char *actionJson=kActionJson);
  [[nodiscard]] ActionModel &getAction(int32_t id);

  void performAction(int32_t id, const std::unique_ptr<IEventObserver> &observer);

  // For things that are dependent on actions having occurred
  bool actionHasHappened(int32_t actionId);
  void setActionsThatHaveAlreadyHappened(std::unordered_set<int32_t> actionIds);

private:
  Game &m_game;
  rapidjson::Document m_actionsDocument;
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
        {"max_mph", 1}
      ]
    }
  ]
  )JSON";
};
}
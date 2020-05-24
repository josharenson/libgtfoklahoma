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
#include <future>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <libgtfoklahoma/stat_model.hpp>
#include <libgtfoklahoma/stats.hpp>

namespace libgtfoklahoma {

class Game;
struct ActionModel {
  explicit ActionModel(Game &game);

  // Actions can be more than one type at a time so use a mask
  enum ActionType { NONE = 0,
                    STAT_CHANGE = 1u << 0u,
                    STORE = 1u << 1u,
                    INVENTORY_DEPENDENT = 1u << 2u
  };

  int32_t id{-1};
  std::vector<std::pair<int32_t, int32_t>> dependent_inventory_ids;
  std::vector<int32_t> ending_id_hints;
  std::string display_name;

  // If action is INVENTORY_CHANGE type
  // Actions aren't visible if the player doesn't meet the inventory requirements
  bool isVisible();

  // If action is STAT_CHANGE type
  float success_chance{0};
  std::string message_failure;
  std::string message_success;
  StatModel stat_delta_regardless;
  StatModel stat_delta_on_success;
  StatModel stat_delta_on_failure;

  // The idea here is the client gets the result before calling `chooseAction`
  // The engine will automatically apply stat changes but its up to the client
  // to inform the player of the result and the failure/success message
  // I guess clients could use this to cheat, but this is MY game and I'll do what I want.
  bool can_fail{false};
  [[nodiscard]] bool failed() const;

  // If action is STORE type
  std::vector<int32_t> item_ids;
  void completePurchase();
  [[nodiscard]] bool itemIsInStock(int32_t itemId) const;
  [[nodiscard]] bool purchaseItem(int32_t id_to_buy);
  std::future<void> purchaseComplete();

  // Helpers for erebody
  uint32_t type{0};
  [[nodiscard]] bool isInventoryDependentType() const { return type & ActionType::INVENTORY_DEPENDENT; }
  [[nodiscard]] bool isNoneType() const { return type & ActionType::NONE; }
  [[nodiscard]] bool isStatChangeType() const { return type & ActionType::STAT_CHANGE; }
  [[nodiscard]] bool isStoreType() const { return type & ActionType::STORE; }

  bool operator==(const ActionModel &rhs) const;

private:
  friend class Actions;
  bool m_successful;

  Game &m_game;
  std::promise<void> m_purchaseComplete;
};
} // namespace libgtfoklahoma

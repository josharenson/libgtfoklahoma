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

#include <libgtfoklahoma/action_model.hpp>

#include <spdlog/spdlog.h>

#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/game.hpp>

using namespace libgtfoklahoma;

ActionModel::ActionModel(Game &game)
: m_successful(false)
, m_game(game) {}

bool ActionModel::isVisible() {
  for (const auto &dependent_inventory_id : dependent_inventory_ids) {
    if (!(m_game.inventoryCount(dependent_inventory_id.first) >=
        dependent_inventory_id.second)) {
      return false;
    }
  }
  return true;
}

bool ActionModel::failed() const {
  return !m_successful;
}

void ActionModel::completePurchase() { m_purchaseComplete.set_value(); }

bool ActionModel::itemIsInStock(int32_t itemId) const {
  if (!isStoreType()) { return false; }
  return std::find(item_ids.begin(), item_ids.end(), itemId) != item_ids.end();
}

bool ActionModel::purchaseItem(int32_t id_to_buy) {
  auto &item = m_game.getItems().getItem(id_to_buy);
  auto &stats = m_game.getStats().getPlayerStatsModel();
  if (item.cost > stats.money_remaining) {
    spdlog::debug("Your broke ass can't afford this item!");
    return false;
  }
  m_game.addItemToInventory(id_to_buy);
  return true;
}

std::future<void> ActionModel::purchaseComplete() {
  return m_purchaseComplete.get_future();
}

bool ActionModel::operator==(const ActionModel &rhs) const {
  return this->id == rhs.id &&
         this->display_name == rhs.display_name &&
         this->item_ids == rhs.item_ids &&
         this->stat_delta_regardless == rhs.stat_delta_regardless &&
         this->type == rhs.type;
}

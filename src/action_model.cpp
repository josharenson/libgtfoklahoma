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

using namespace libgtfoklahoma;

void ActionModel::completePurchase() {
  m_purchasedItems.set_value(m_itemsPendingPurchase);
}

void ActionModel::purchaseItem(int32_t id_to_buy) {
  m_itemsPendingPurchase.push_back(id_to_buy);
}

std::future<std::vector<int32_t>> ActionModel::purchasedItems() {
  return m_purchasedItems.get_future();
}
bool ActionModel::operator==(const ActionModel &rhs) const {
  return this->id == rhs.id &&
         this->display_name == rhs.display_name &&
         this->item_ids == rhs.item_ids &&
         this->stat_delta == rhs.stat_delta &&
         this->type == rhs.type;
}
